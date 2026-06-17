#include "audio_input.h"

#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2s_std.h"
#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "audio_input";

// 当前 INMP441 接线
#define INMP441_BCLK   4
#define INMP441_WS     5
#define INMP441_DIN    6

// 后续接 ESP-SR，16kHz 是常用输入采样率
#define INMP441_SAMPLE_RATE_HZ  16000

// 每次从 I2S 读取的最大 32-bit 原始样本数
#define AUDIO_RAW_READ_SAMPLES  512

/*
 * INMP441 是 24-bit I2S 数字麦克风，读入时放在 32-bit slot 中。
 *
 * 当前你测试时 raw >> 14 得到的 RMS 效果正常：
 * 静音约 40~80，有声音可到 2000+。
 *
 * 所以 PCM16 先沿用右移 14 位。
 * 如果后续接 ESP-SR 时发现声音过小，可以改成 13；
 * 如果发现容易爆音/饱和，可以改成 15 或 16。
 */
#define INMP441_PCM_SHIFT  14

static i2s_chan_handle_t s_i2s_rx_chan = NULL;

static inline int16_t clamp_int16(int32_t value)
{
    if (value > INT16_MAX) {
        return INT16_MAX;
    }

    if (value < INT16_MIN) {
        return INT16_MIN;
    }

    return (int16_t)value;
}

esp_err_t audio_input_init(void)
{
    if (s_i2s_rx_chan != NULL) {
        ESP_LOGW(TAG, "I2S already initialized");
        return ESP_OK;
    }

    i2s_chan_config_t chan_cfg =
        I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);

    ESP_RETURN_ON_ERROR(
        i2s_new_channel(&chan_cfg, NULL, &s_i2s_rx_chan),
        TAG,
        "i2s_new_channel failed"
    );

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(INMP441_SAMPLE_RATE_HZ),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_32BIT,
            I2S_SLOT_MODE_STEREO
        ),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = INMP441_BCLK,
            .ws   = INMP441_WS,
            .dout = I2S_GPIO_UNUSED,
            .din  = INMP441_DIN,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    /*
     * 你的 INMP441 L/R 接 GND，所以麦克风输出在左声道。
     * 如果以后 L/R 接 3V3，需要改成 I2S_STD_SLOT_RIGHT。
     */
    std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;

    ESP_RETURN_ON_ERROR(
        i2s_channel_init_std_mode(s_i2s_rx_chan, &std_cfg),
        TAG,
        "i2s_channel_init_std_mode failed"
    );

    ESP_RETURN_ON_ERROR(
        i2s_channel_enable(s_i2s_rx_chan),
        TAG,
        "i2s_channel_enable failed"
    );

    ESP_LOGI(
        TAG,
        "INMP441 initialized: BCLK=%d, WS=%d, DIN=%d, sample_rate=%d",
        INMP441_BCLK,
        INMP441_WS,
        INMP441_DIN,
        INMP441_SAMPLE_RATE_HZ
    );

    return ESP_OK;
}

esp_err_t audio_input_read_pcm16(int16_t *out_buf, size_t sample_count, size_t *samples_read)
{
    if (out_buf == NULL || samples_read == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_i2s_rx_chan == NULL) {
        ESP_LOGE(TAG, "I2S not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    *samples_read = 0;

    while (*samples_read < sample_count) {
        size_t remain = sample_count - *samples_read;
        size_t this_read_samples = remain;

        if (this_read_samples > AUDIO_RAW_READ_SAMPLES) {
            this_read_samples = AUDIO_RAW_READ_SAMPLES;
        }

        int32_t raw_buf[AUDIO_RAW_READ_SAMPLES] = {0};
        size_t bytes_read = 0;

        esp_err_t ret = i2s_channel_read(
            s_i2s_rx_chan,
            raw_buf,
            this_read_samples * sizeof(int32_t),
            &bytes_read,
            pdMS_TO_TICKS(1000)
        );

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "i2s_channel_read failed: %s", esp_err_to_name(ret));
            return ret;
        }

        size_t got_samples = bytes_read / sizeof(int32_t);

        if (got_samples == 0) {
            break;
        }

        for (size_t i = 0; i < got_samples; i++) {
            int32_t sample32 = raw_buf[i] >> INMP441_PCM_SHIFT;
            out_buf[*samples_read + i] = clamp_int16(sample32);
        }

        *samples_read += got_samples;
    }

    return ESP_OK;
}

esp_err_t audio_input_read_rms(int *out_rms)
{
    if (out_rms == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    int16_t pcm_buf[AUDIO_RAW_READ_SAMPLES] = {0};
    size_t samples_read = 0;

    esp_err_t ret = audio_input_read_pcm16(
        pcm_buf,
        AUDIO_RAW_READ_SAMPLES,
        &samples_read
    );

    if (ret != ESP_OK) {
        return ret;
    }

    if (samples_read == 0) {
        *out_rms = 0;
        return ESP_OK;
    }

    int64_t sum_sq = 0;

    for (size_t i = 0; i < samples_read; i++) {
        int32_t sample = pcm_buf[i];
        sum_sq += (int64_t)sample * sample;
    }

    *out_rms = (int)sqrt((double)sum_sq / samples_read);

    return ESP_OK;
}