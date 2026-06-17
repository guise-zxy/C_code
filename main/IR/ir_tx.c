#include "ir_tx.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/rmt_tx.h"
#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "ir_tx";

#define IR_TX_GPIO              GPIO_NUM_7
#define IR_TX_RESOLUTION_HZ     1000000U  // 1 tick = 1 us
#define IR_CARRIER_FREQ_HZ      38000U
#define IR_CARRIER_DUTY_CYCLE   0.33f

// 美的空调 R05D 时序，单位：us
#define R05D_LEAD_MARK_US       4400U
#define R05D_LEAD_SPACE_US      4400U
#define R05D_STOP_MARK_US       540U
#define R05D_STOP_SPACE_US      5220U
#define R05D_BIT_MARK_US        540U
#define R05D_ZERO_SPACE_US      540U
#define R05D_ONE_SPACE_US       1620U

// 每帧：1 个引导码 + 6 字节 * 8 bit + 1 个分隔/结束码；发送两帧
#define R05D_SYMBOL_COUNT       100U

static rmt_channel_handle_t s_tx_channel = NULL;
static rmt_encoder_handle_t s_copy_encoder = NULL;

static bool append_symbol(
    rmt_symbol_word_t *symbols,
    size_t capacity,
    size_t *count,
    uint16_t mark_us,
    uint16_t space_us
)
{
    if (symbols == NULL || count == NULL || *count >= capacity) {
        return false;
    }

    symbols[*count] = (rmt_symbol_word_t) {
        .level0 = 1,
        .duration0 = mark_us,
        .level1 = 0,
        .duration1 = space_us,
    };

    (*count)++;
    return true;
}

static bool append_byte_msb_first(
    rmt_symbol_word_t *symbols,
    size_t capacity,
    size_t *count,
    uint8_t data
)
{
    for (int bit = 7; bit >= 0; bit--) {
        const bool is_one = (data & (1U << bit)) != 0;
        const uint16_t space_us = is_one ? R05D_ONE_SPACE_US : R05D_ZERO_SPACE_US;

        if (!append_symbol(symbols, capacity, count, R05D_BIT_MARK_US, space_us)) {
            return false;
        }
    }

    return true;
}

static esp_err_t transmit_symbols(
    const rmt_symbol_word_t *symbols,
    size_t symbol_count
)
{
    ESP_RETURN_ON_FALSE(
        s_tx_channel != NULL && s_copy_encoder != NULL,
        ESP_ERR_INVALID_STATE,
        TAG,
        "IR TX is not initialized"
    );

    rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
        .flags = {
            .eot_level = 0,
            .queue_nonblocking = false,
        },
    };

    ESP_RETURN_ON_ERROR(
        rmt_transmit(
            s_tx_channel,
            s_copy_encoder,
            symbols,
            symbol_count * sizeof(symbols[0]),
            &transmit_config
        ),
        TAG,
        "rmt_transmit failed"
    );

    ESP_RETURN_ON_ERROR(
        rmt_tx_wait_all_done(s_tx_channel, 1000),
        TAG,
        "rmt_tx_wait_all_done failed"
    );

    return ESP_OK;
}

esp_err_t ir_tx_init(void)
{
    if (s_tx_channel != NULL) {
        return ESP_OK;
    }

    rmt_tx_channel_config_t tx_channel_config = {
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
        rmt_new_tx_channel(&tx_channel_config, &s_tx_channel),
        TAG,
        "rmt_new_tx_channel failed"
    );

    rmt_carrier_config_t carrier_config = {
        .frequency_hz = IR_CARRIER_FREQ_HZ,
        .duty_cycle = IR_CARRIER_DUTY_CYCLE,
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

rmt_copy_encoder_config_t copy_encoder_config;

ESP_RETURN_ON_ERROR(
    rmt_new_copy_encoder(&copy_encoder_config, &s_copy_encoder),
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
        (unsigned long) IR_CARRIER_FREQ_HZ
    );

    return ESP_OK;
}

esp_err_t ir_tx_send_camera_test(void)
{
    static const rmt_symbol_word_t test_symbols[] = {
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
        { .level0 = 1, .duration0 = 10000, .level1 = 0, .duration1 = 10000 },
    };

    ESP_RETURN_ON_ERROR(
        transmit_symbols(test_symbols, sizeof(test_symbols) / sizeof(test_symbols[0])),
        TAG,
        "camera test transmit failed"
    );

    ESP_LOGI(TAG, "IR camera test burst sent");
    return ESP_OK;
}

esp_err_t ir_tx_send_midea_r05d(uint8_t a, uint8_t b, uint8_t c)
{
    rmt_symbol_word_t symbols[R05D_SYMBOL_COUNT] = {0};
    size_t symbol_count = 0;

    const uint8_t payload[] = {
        a,
        (uint8_t) ~a,
        b,
        (uint8_t) ~b,
        c,
        (uint8_t) ~c,
    };

    for (int frame = 0; frame < 2; frame++) {
        ESP_RETURN_ON_FALSE(
            append_symbol(
                symbols,
                R05D_SYMBOL_COUNT,
                &symbol_count,
                R05D_LEAD_MARK_US,
                R05D_LEAD_SPACE_US
            ),
            ESP_ERR_INVALID_SIZE,
            TAG,
            "failed to append R05D lead symbol"
        );

        for (size_t i = 0; i < sizeof(payload); i++) {
            ESP_RETURN_ON_FALSE(
                append_byte_msb_first(symbols, R05D_SYMBOL_COUNT, &symbol_count, payload[i]),
                ESP_ERR_INVALID_SIZE,
                TAG,
                "failed to append R05D byte"
            );
        }

        ESP_RETURN_ON_FALSE(
            append_symbol(
                symbols,
                R05D_SYMBOL_COUNT,
                &symbol_count,
                R05D_STOP_MARK_US,
                R05D_STOP_SPACE_US
            ),
            ESP_ERR_INVALID_SIZE,
            TAG,
            "failed to append R05D stop symbol"
        );
    }

    ESP_RETURN_ON_FALSE(
        symbol_count == R05D_SYMBOL_COUNT,
        ESP_ERR_INVALID_SIZE,
        TAG,
        "unexpected R05D symbol count: %u",
        (unsigned int) symbol_count
    );

    ESP_RETURN_ON_ERROR(
        transmit_symbols(symbols, symbol_count),
        TAG,
        "R05D transmit failed"
    );

    ESP_LOGI(TAG, "R05D sent: A=0x%02X, B=0x%02X, C=0x%02X", a, b, c);
    return ESP_OK;
}

esp_err_t ir_tx_send_midea_power_on_test(void)
{
    // 博客验证示例：自动风、制冷模式、25 摄氏度
    return ir_tx_send_midea_r05d(0xB2, 0xBF, 0xC0);
}

esp_err_t ir_tx_send_midea_power_off(void)
{
    // R05D 固定关机帧
    return ir_tx_send_midea_r05d(0xB2, 0x7B, 0xE0);
}
