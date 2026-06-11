#include "ir_tx.h"

#include "driver/rmt_tx.h"
#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "ir_tx";

#define IR_TX_GPIO              7
#define IR_TX_RESOLUTION_HZ     1000000U   // 1 MHz，1 tick = 1 us
#define IR_CARRIER_FREQ_HZ      38000U     // 常见红外遥控载波
#define IR_CARRIER_DUTY         0.33f

static rmt_channel_handle_t s_tx_channel = NULL;
static rmt_encoder_handle_t s_copy_encoder = NULL;

esp_err_t ir_tx_init(void)
{
    if (s_tx_channel != NULL) {
        return ESP_OK;
    }

    rmt_tx_channel_config_t tx_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = IR_TX_GPIO,
        .mem_block_symbols = 64,
        .resolution_hz = IR_TX_RESOLUTION_HZ,
        .trans_queue_depth = 4,
        .flags = {
            .invert_out = false,
            .with_dma = false,
        },
    };

    ESP_RETURN_ON_ERROR(
        rmt_new_tx_channel(&tx_config, &s_tx_channel),
        TAG,
        "rmt_new_tx_channel failed"
    );

    rmt_carrier_config_t carrier_config = {
        .frequency_hz = IR_CARRIER_FREQ_HZ,
        .duty_cycle = IR_CARRIER_DUTY,
        .flags = {
            .polarity_active_low = false,
            .always_on = false,
        },
    };

    ESP_RETURN_ON_ERROR(
        rmt_apply_carrier(s_tx_channel, &carrier_config),
        TAG,
        "rmt_apply_carrier failed"
    );

    rmt_copy_encoder_config_t encoder_config = {};

    ESP_RETURN_ON_ERROR(
        rmt_new_copy_encoder(&encoder_config, &s_copy_encoder),
        TAG,
        "rmt_new_copy_encoder failed"
    );

    ESP_RETURN_ON_ERROR(
        rmt_enable(s_tx_channel),
        TAG,
        "rmt_enable failed"
    );

    ESP_LOGI(
        TAG,
        "IR TX initialized: GPIO=%d, carrier=%lu Hz",
        IR_TX_GPIO,
        (unsigned long)IR_CARRIER_FREQ_HZ
    );

    return ESP_OK;
}

esp_err_t ir_tx_send_camera_test(void)
{
    if (s_tx_channel == NULL || s_copy_encoder == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    /*
     * 每个 symbol：
     * 20 ms 发射 38 kHz 载波
     * 20 ms 停止发射
     *
     * 一共持续约 400 ms，方便手机摄像头观察。
     */
    static const rmt_symbol_word_t test_symbols[] = {
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
        { .level0 = 1, .duration0 = 20000, .level1 = 0, .duration1 = 20000 },
    };

    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
    };

    ESP_RETURN_ON_ERROR(
        rmt_transmit(
            s_tx_channel,
            s_copy_encoder,
            test_symbols,
            sizeof(test_symbols),
            &tx_config
        ),
        TAG,
        "rmt_transmit failed"
    );

    ESP_RETURN_ON_ERROR(
        rmt_tx_wait_all_done(s_tx_channel, 1000),
        TAG,
        "rmt_tx_wait_all_done failed"
    );

    ESP_LOGI(TAG, "IR camera test burst sent");

    return ESP_OK;
}