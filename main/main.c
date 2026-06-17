#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

#include "audio_input.h"
#include "ir_tx.h"

// ESP-SR
#include "esp_process_sdkconfig.h"
#include "esp_afe_config.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "model_path.h"

static const char *TAG = "sleep_sr";

static const esp_afe_sr_iface_t *s_afe_handle = NULL;
static esp_afe_sr_data_t *s_afe_data = NULL;
static srmodel_list_t *s_models = NULL;

static volatile bool s_task_running = true;
static volatile bool s_wakeup_flag = false;

#define RMS_LOG_INTERVAL_FRAMES 50


/**
 * R05D 临时连续测试开关：
 * - R05D_TEST_POWER_ON：每 2 秒重复发送一次开机状态帧；
 * - R05D_TEST_POWER_OFF：每 2 秒重复发送一次关机帧；
 * - R05D_TEST_DISABLED：不发送测试帧。
 *
 * 仅用于快速验证红外链路。测试完成后应恢复为按需发送。
 * 不要自动交替发送开机和关机，避免空调频繁切换状态。
 */
#define R05D_TEST_DISABLED  0
#define R05D_TEST_POWER_ON   1
#define R05D_TEST_POWER_OFF  2

#define R05D_TEST_ACTION R05D_TEST_DISABLED
#define R05D_TEST_INTERVAL_MS 2000

#if R05D_TEST_ACTION != R05D_TEST_DISABLED
static void ir_r05d_test_task(void *arg)
{
    (void)arg;

    esp_err_t ret = ir_tx_init();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ir_tx_init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    /* 上电后稍等片刻，便于观察串口启动日志。 */
    vTaskDelay(pdMS_TO_TICKS(1000));

    TickType_t last_wake_time = xTaskGetTickCount();
    uint32_t send_count = 0;

    while (true) {
        if (R05D_TEST_ACTION == R05D_TEST_POWER_ON) {
            ESP_LOGI(TAG, "Sending Midea R05D POWER ON test: cool mode, auto fan, 25 C");
            ret = ir_tx_send_midea_power_on_test();
        } else if (R05D_TEST_ACTION == R05D_TEST_POWER_OFF) {
            ESP_LOGI(TAG, "Sending Midea R05D POWER OFF test");
            ret = ir_tx_send_midea_power_off();
        } else {
            ESP_LOGI(TAG, "R05D test disabled");
            vTaskDelete(NULL);
            return;
        }

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "R05D test transmit failed: %s", esp_err_to_name(ret));
        } else {
            send_count++;
            ESP_LOGI(TAG, "R05D periodic test sent: count=%lu", (unsigned long)send_count);
        }

        /* 保持约每 2 秒发送一次，而不是“发送完成后再额外等待 2 秒”。 */
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(R05D_TEST_INTERVAL_MS));
    }
}
#endif

#define VOICE_CMD_AC_POWER_ON  0
#define VOICE_CMD_AC_POWER_OFF 1

static void handle_voice_command(int command_id)
{
    esp_err_t ret = ESP_OK;

    switch (command_id) {
        case VOICE_CMD_AC_POWER_ON:
            ESP_LOGI(TAG, "[VOICE CMD] AC power on");
            ret = ir_tx_send_midea_power_on_test();
            break;

        case VOICE_CMD_AC_POWER_OFF:
            ESP_LOGI(TAG, "[VOICE CMD] AC power off");
            ret = ir_tx_send_midea_power_off();
            break;

        default:
            ESP_LOGW(TAG, "[VOICE CMD] Unbound command_id=%d", command_id);
            return;
    }

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "[VOICE CMD] IR transmit failed: %s", esp_err_to_name(ret));
        return;
    }

    ESP_LOGI(TAG, "[VOICE CMD] IR transmit done");
}

/**
 * 用于确认音频仍然正常进入 ESP-SR。
 * 不参与识别逻辑，只做低频调试输出。
 */
static int calculate_rms(const int16_t *buffer, size_t sample_count)
{
    if (buffer == NULL || sample_count == 0) {
        return 0;
    }

    int64_t sum_sq = 0;

    for (size_t i = 0; i < sample_count; i++) {
        int32_t sample = buffer[i];
        sum_sq += (int64_t)sample * sample;
    }

    return (int)sqrt((double)sum_sq / sample_count);
}

/**
 * 音频采集任务：
 * INMP441 → PCM16 → AFE feed()
 */
static void feed_task(void *arg)
{
    esp_afe_sr_data_t *afe_data = (esp_afe_sr_data_t *)arg;

    const int feed_chunksize = s_afe_handle->get_feed_chunksize(afe_data);
    const int feed_channel_num = s_afe_handle->get_feed_channel_num(afe_data);

    ESP_LOGI(
        TAG,
        "AFE feed started: chunk=%d, channels=%d",
        feed_chunksize,
        feed_channel_num
    );

    /*
     * 当前使用一个 INMP441，因此 input_format = "M"，
     * AFE 应要求一个输入通道。
     */
    if (feed_channel_num != 1) {
        ESP_LOGE(
            TAG,
            "Expected one microphone channel, but AFE requested %d channels",
            feed_channel_num
        );
        vTaskDelete(NULL);
        return;
    }

    int16_t *pcm_buffer =
        (int16_t *)malloc(feed_chunksize * sizeof(int16_t));

    assert(pcm_buffer != NULL);

    uint32_t frame_count = 0;

    while (s_task_running) {
        size_t samples_read = 0;

        esp_err_t ret = audio_input_read_pcm16(
            pcm_buffer,
            feed_chunksize,
            &samples_read
        );

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "audio_input_read_pcm16 failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (samples_read != (size_t)feed_chunksize) {
            ESP_LOGW(
                TAG,
                "short PCM read: expected=%d, actual=%u",
                feed_chunksize,
                (unsigned)samples_read
            );
            continue;
        }

        /*
         * 每隔一段时间打印一次 RMS，确认录音仍然正常。
         * 说话时，数值应明显高于静音状态。
         */
        frame_count++;

        if (frame_count % RMS_LOG_INTERVAL_FRAMES == 0) {
            ESP_LOGI(TAG, "AFE input rms = %d", calculate_rms(pcm_buffer, samples_read));
        }

        s_afe_handle->feed(afe_data, pcm_buffer);
    }

    free(pcm_buffer);
    vTaskDelete(NULL);
}

/**
 * 检测任务：
 * AFE fetch() → WakeNet → MultiNet
 */
static void detect_task(void *arg)
{
    esp_afe_sr_data_t *afe_data = (esp_afe_sr_data_t *)arg;

    const int afe_chunksize = s_afe_handle->get_fetch_chunksize(afe_data);

    char *mn_name =
        esp_srmodel_filter(s_models, ESP_MN_PREFIX, ESP_MN_CHINESE);

    if (mn_name == NULL) {
        ESP_LOGE(TAG, "No Chinese MultiNet model found");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "MultiNet model: %s", mn_name);

    esp_mn_iface_t *multinet = esp_mn_handle_from_name(mn_name);

    if (multinet == NULL) {
        ESP_LOGE(TAG, "esp_mn_handle_from_name failed");
        vTaskDelete(NULL);
        return;
    }

    model_iface_data_t *model_data = multinet->create(mn_name, 6000);

    if (model_data == NULL) {
        ESP_LOGE(TAG, "MultiNet create failed");
        vTaskDelete(NULL);
        return;
    }

    /*
     * 从 menuconfig 载入固定中文命令词。
     */
  esp_mn_commands_update_from_sdkconfig(multinet, model_data);

    const int mn_chunksize = multinet->get_samp_chunksize(model_data);

    ESP_LOGI(
        TAG,
        "AFE fetch chunk=%d, MultiNet chunk=%d",
        afe_chunksize,
        mn_chunksize
    );

    assert(mn_chunksize == afe_chunksize);

    multinet->print_active_speech_commands(model_data);

    ESP_LOGI(TAG, "Detection started. Say wake word first.");

    while (s_task_running) {
        afe_fetch_result_t *result = s_afe_handle->fetch(afe_data);

        if (result == NULL || result->ret_value == ESP_FAIL) {
            ESP_LOGE(TAG, "AFE fetch failed");
            break;
        }

        if (result->wakeup_state == WAKENET_DETECTED) {
            ESP_LOGI(TAG, "WAKEWORD DETECTED");
            multinet->clean(model_data);
        }

        /*
         * 单麦克风场景：检测到 WakeNet 后即可进入命令词识别。
         */
        if (
            result->raw_data_channels == 1 &&
            result->wakeup_state == WAKENET_DETECTED
        ) {
            s_wakeup_flag = true;
            ESP_LOGI(TAG, "Listening for speech command...");
        }

        /*
         * 双麦场景以后才会用到：
         * 需要等待通道验证完成。
         */
        if (
            result->raw_data_channels > 1 &&
            result->wakeup_state == WAKENET_CHANNEL_VERIFIED
        ) {
            s_wakeup_flag = true;

            ESP_LOGI(
                TAG,
                "WakeNet channel verified: channel=%d",
                result->trigger_channel_id
            );
        }

        if (!s_wakeup_flag) {
            continue;
        }

        esp_mn_state_t mn_state =
            multinet->detect(model_data, result->data);

        if (mn_state == ESP_MN_STATE_DETECTING) {
            continue;
        }

        if (mn_state == ESP_MN_STATE_DETECTED) {
            esp_mn_results_t *mn_result =
                multinet->get_results(model_data);

            if (mn_result != NULL) {
                for (int i = 0; i < mn_result->num; i++) {
                    ESP_LOGI(
                        TAG,
                        "TOP %d: command_id=%d, phrase_id=%d, string=%s, prob=%f",
                        i + 1,
                        mn_result->command_id[i],
                        mn_result->phrase_id[i],
                        mn_result->string,
                        mn_result->prob[i]
                    );
                }

                if (mn_result->num > 0) {
                    handle_voice_command(mn_result->command_id[0]);
                    multinet->clean(model_data);
                    s_afe_handle->enable_wakenet(afe_data);
                    s_wakeup_flag = false;
                    ESP_LOGI(TAG, "Command handled. Waiting for wake word.");
                    continue;
                }
            }

            ESP_LOGI(TAG, "Still listening until timeout...");
        }

        if (mn_state == ESP_MN_STATE_TIMEOUT) {
            ESP_LOGI(TAG, "MultiNet timeout. Waiting for wake word.");

            s_afe_handle->enable_wakenet(afe_data);
            s_wakeup_flag = false;
        }
    }

    multinet->destroy(model_data);
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Sleep Agent voice-controlled IR start");

    ESP_ERROR_CHECK(ir_tx_init());

    /*
     * 已经单独验证通过：
     * INMP441 → I2S → PCM16
     */
    ESP_ERROR_CHECK(audio_input_init());

    /*
     * 从 model 分区读取 menuconfig 中选择的模型。
     */
    s_models = esp_srmodel_init("model");

    if (s_models == NULL) {
        ESP_LOGE(
            TAG,
            "esp_srmodel_init failed. Check partitions.csv and full flash."
        );
        return;
    }

    /*
     * M = 单个麦克风通道。
     * 当前没有扬声器播放参考通道，因此不使用 R。
     */
    afe_config_t *afe_config = afe_config_init(
        "M",
        s_models,
        AFE_TYPE_SR,
        AFE_MODE_LOW_COST
    );

    if (afe_config == NULL) {
        ESP_LOGE(TAG, "afe_config_init failed");
        return;
    }

    s_afe_handle = esp_afe_handle_from_config(afe_config);

    if (s_afe_handle == NULL) {
        ESP_LOGE(TAG, "esp_afe_handle_from_config failed");
        afe_config_free(afe_config);
        return;
    }

    s_afe_data = s_afe_handle->create_from_config(afe_config);

    afe_config_free(afe_config);

    if (s_afe_data == NULL) {
        ESP_LOGE(TAG, "AFE create_from_config failed");
        return;
    }

    BaseType_t detect_created = xTaskCreatePinnedToCore(
        detect_task,
        "sr_detect",
        8 * 1024,
        s_afe_data,
        5,
        NULL,
        1
    );

    BaseType_t feed_created = xTaskCreatePinnedToCore(
        feed_task,
        "sr_feed",
        8 * 1024,
        s_afe_data,
        5,
        NULL,
        0
    );

    if (detect_created != pdPASS || feed_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create ESP-SR tasks");
        return;
    }

    ESP_LOGI(TAG, "ESP-SR tasks created");
}
