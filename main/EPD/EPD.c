#include "EPD.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "epd";

/*
 * 当前接线：
 * EPD DIN  -> GPIO11  // SPI MOSI
 * EPD CLK  -> GPIO12  // SPI SCLK
 * EPD CS   -> GPIO10  // SPI CS
 * EPD DC   -> GPIO13  // 命令/数据选择
 * EPD RST  -> GPIO14  // 复位
 * EPD BUSY -> GPIO21  // 忙状态输入
 */
#define EPD_HOST        SPI2_HOST

#define EPD_PIN_MOSI    11
#define EPD_PIN_SCLK    12
#define EPD_PIN_CS      10
#define EPD_PIN_DC      13
#define EPD_PIN_RST     14
#define EPD_PIN_BUSY    21
#define EPD_PIN_MISO    -1

/*
 * Waveshare 4.2inch e-Paper V2 官方代码中：
 * LOW: idle, HIGH: busy
 */
#define EPD_BUSY_ACTIVE_LEVEL  1

#define EPD_WAIT_TIMEOUT_MS    10000

static spi_device_handle_t epd_spi = NULL;

#define EPD_RETURN_ON_ERROR(expr) do {                  \
    esp_err_t __ret = (expr);                           \
    if (__ret != ESP_OK) {                              \
        ESP_LOGE(TAG, "%s failed: %s", #expr, esp_err_to_name(__ret)); \
        return __ret;                                   \
    }                                                   \
} while (0)

esp_err_t epd_gpio_init(void)
{
    gpio_config_t output_conf = {
        .pin_bit_mask = (1ULL << EPD_PIN_DC) | (1ULL << EPD_PIN_RST),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    esp_err_t ret = gpio_config(&output_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to config DC/RST GPIO, ret=%s", esp_err_to_name(ret));
        return ret;
    }

    gpio_config_t busy_conf = {
        .pin_bit_mask = (1ULL << EPD_PIN_BUSY),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ret = gpio_config(&busy_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to config BUSY GPIO, ret=%s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

esp_err_t epd_spi_bus_init(void)
{
    if (epd_spi != NULL) {
        ESP_LOGW(TAG, "EPD SPI device already initialized");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing SPI bus");

    spi_bus_config_t buscfg = {
        .miso_io_num = EPD_PIN_MISO,
        .mosi_io_num = EPD_PIN_MOSI,
        .sclk_io_num = EPD_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = EPD_BUF_SIZE,
    };

    esp_err_t ret = spi_bus_initialize(EPD_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPI bus, ret=%s", esp_err_to_name(ret));
        return ret;
    }

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 2 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = EPD_PIN_CS,
        .queue_size = 1,
    };

    ret = spi_bus_add_device(EPD_HOST, &devcfg, &epd_spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add EPD SPI device, ret=%s", esp_err_to_name(ret));
        spi_bus_free(EPD_HOST);
        return ret;
    }

    ESP_LOGI(TAG, "EPD SPI bus init ok");

    return ESP_OK;
}

static esp_err_t epd_spi_write(const uint8_t *data, size_t len)
{
    if (epd_spi == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (data == NULL || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    spi_transaction_t trans = {
        .length = len * 8,
        .tx_buffer = data,
    };

    return spi_device_transmit(epd_spi, &trans);
}

esp_err_t epd_send_cmd(uint8_t cmd)
{
    gpio_set_level(EPD_PIN_DC, 0);
    return epd_spi_write(&cmd, 1);
}

esp_err_t epd_send_data(uint8_t data)
{
    gpio_set_level(EPD_PIN_DC, 1);
    return epd_spi_write(&data, 1);
}

esp_err_t epd_send_data_buffer(const uint8_t *data, size_t len)
{
    gpio_set_level(EPD_PIN_DC, 1);
    return epd_spi_write(data, len);
}

void epd_reset(void)
{
    ESP_LOGI(TAG, "Resetting EPD");

    /*
     * 按 Waveshare V2 官方驱动思路：
     * RST 先拉高，再短暂拉低，再拉高释放复位。
     */
    gpio_set_level(EPD_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    gpio_set_level(EPD_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(2));

    gpio_set_level(EPD_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "EPD reset done");
}

int epd_get_busy_level(void)
{
    return gpio_get_level(EPD_PIN_BUSY);
}

esp_err_t epd_wait_busy_timeout(uint32_t timeout_ms)
{
    uint32_t elapsed_ms = 0;

    ESP_LOGI(TAG, "Waiting EPD BUSY, active_level=%d", EPD_BUSY_ACTIVE_LEVEL);

    while (gpio_get_level(EPD_PIN_BUSY) == EPD_BUSY_ACTIVE_LEVEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
        elapsed_ms += 10;

        if (elapsed_ms >= timeout_ms) {
            ESP_LOGE(TAG,
                     "EPD BUSY timeout after %lu ms, current level=%d",
                     (unsigned long)elapsed_ms,
                     gpio_get_level(EPD_PIN_BUSY));
            return ESP_ERR_TIMEOUT;
        }
    }

    ESP_LOGI(TAG, "EPD BUSY released, level=%d", gpio_get_level(EPD_PIN_BUSY));

    return ESP_OK;
}

static esp_err_t epd_set_windows(uint16_t x_start, uint16_t y_start,
                                 uint16_t x_end, uint16_t y_end)
{
    /*
     * 0x44: 设置 X 地址起止位置，单位是 byte，所以 X 要 /8。
     * 400 像素宽，对应 50 byte。
     */
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x44));
    EPD_RETURN_ON_ERROR(epd_send_data((x_start >> 3) & 0xFF));
    EPD_RETURN_ON_ERROR(epd_send_data((x_end >> 3) & 0xFF));

    /*
     * 0x45: 设置 Y 地址起止位置，Y 是 16 位。
     */
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x45));
    EPD_RETURN_ON_ERROR(epd_send_data(y_start & 0xFF));
    EPD_RETURN_ON_ERROR(epd_send_data((y_start >> 8) & 0xFF));
    EPD_RETURN_ON_ERROR(epd_send_data(y_end & 0xFF));
    EPD_RETURN_ON_ERROR(epd_send_data((y_end >> 8) & 0xFF));

    return ESP_OK;
}

static esp_err_t epd_set_cursor(uint16_t x_start, uint16_t y_start)
{
    /*
     * 0x4E: 设置 X 地址计数器。
     * 0x4F: 设置 Y 地址计数器。
     */
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x4E));
    EPD_RETURN_ON_ERROR(epd_send_data((x_start >> 3) & 0xFF));

    EPD_RETURN_ON_ERROR(epd_send_cmd(0x4F));
    EPD_RETURN_ON_ERROR(epd_send_data(y_start & 0xFF));
    EPD_RETURN_ON_ERROR(epd_send_data((y_start >> 8) & 0xFF));

    return ESP_OK;
}

static esp_err_t epd_turn_on_display(void)
{
    /*
     * 0x22: Display Update Control 2
     * 0xF7: Waveshare V2 全刷使用值
     * 0x20: Master Activation，开始刷新
     */
    ESP_LOGI(TAG, "Turn on display refresh");

    EPD_RETURN_ON_ERROR(epd_send_cmd(0x22));
    EPD_RETURN_ON_ERROR(epd_send_data(0xF7));

    EPD_RETURN_ON_ERROR(epd_send_cmd(0x20));

    return epd_wait_busy_timeout(EPD_WAIT_TIMEOUT_MS);
}

esp_err_t epd_init(void)
{
    ESP_LOGI(TAG, "EPD init start");

    epd_reset();

    EPD_RETURN_ON_ERROR(epd_wait_busy_timeout(EPD_WAIT_TIMEOUT_MS));

    /*
     * 0x12: Software Reset
     */
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x12));
    EPD_RETURN_ON_ERROR(epd_wait_busy_timeout(EPD_WAIT_TIMEOUT_MS));

    /*
     * 0x21: Display Update Control
     */
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x21));
    EPD_RETURN_ON_ERROR(epd_send_data(0x40));
    EPD_RETURN_ON_ERROR(epd_send_data(0x00));

    /*
     * 0x3C: Border waveform
     */
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x3C));
    EPD_RETURN_ON_ERROR(epd_send_data(0x05));

    /*
     * 0x11: Data Entry Mode
     * 0x03: X 增加，Y 增加
     */
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x11));
    EPD_RETURN_ON_ERROR(epd_send_data(0x03));

    EPD_RETURN_ON_ERROR(epd_set_windows(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1));
    EPD_RETURN_ON_ERROR(epd_set_cursor(0, 0));

    EPD_RETURN_ON_ERROR(epd_wait_busy_timeout(EPD_WAIT_TIMEOUT_MS));

    ESP_LOGI(TAG, "EPD init done");

    return ESP_OK;
}

static esp_err_t epd_write_full_ram(uint8_t color)
{
    /*
     * 400 * 300 / 8 = 15000 bytes
     * 0xFF 表示白，0x00 表示黑。
     */
    static uint8_t fill_buf[256];

    memset(fill_buf, color, sizeof(fill_buf));

    size_t remaining = EPD_BUF_SIZE;

    while (remaining > 0) {
        size_t chunk = remaining > sizeof(fill_buf) ? sizeof(fill_buf) : remaining;
        EPD_RETURN_ON_ERROR(epd_send_data_buffer(fill_buf, chunk));
        remaining -= chunk;
    }

    return ESP_OK;
}

esp_err_t epd_display_frame(const uint8_t *frame_buffer)
{
    if (frame_buffer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "EPD display frame start, bytes=%u", (unsigned int)EPD_BUF_SIZE);

    /*
     * A full-frame write must restart from the top-left corner.
     * 0x24 and 0x26 follow Waveshare 4.2-inch V2 full-refresh flow.
     */
    EPD_RETURN_ON_ERROR(epd_set_windows(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1));
    EPD_RETURN_ON_ERROR(epd_set_cursor(0, 0));

    EPD_RETURN_ON_ERROR(epd_send_cmd(0x24));
    EPD_RETURN_ON_ERROR(epd_send_data_buffer(frame_buffer, EPD_BUF_SIZE));

    EPD_RETURN_ON_ERROR(epd_set_cursor(0, 0));
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x26));
    EPD_RETURN_ON_ERROR(epd_send_data_buffer(frame_buffer, EPD_BUF_SIZE));

    EPD_RETURN_ON_ERROR(epd_turn_on_display());

    ESP_LOGI(TAG, "EPD display frame done");
    return ESP_OK;
}

esp_err_t epd_clear(uint8_t color)
{
    ESP_LOGI(TAG, "EPD clear start, color=0x%02X", color);

    /*
     * 0x24: Write RAM black/white image data
     */
    EPD_RETURN_ON_ERROR(epd_set_windows(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1));
    EPD_RETURN_ON_ERROR(epd_set_cursor(0, 0));
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x24));
    EPD_RETURN_ON_ERROR(epd_write_full_ram(color));

    /*
     * 0x26: Write RAM red/second image data
     * 对黑白 V2 官方例程，clear/display 时也写这一块。
     */
    EPD_RETURN_ON_ERROR(epd_set_cursor(0, 0));
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x26));
    EPD_RETURN_ON_ERROR(epd_write_full_ram(color));

    EPD_RETURN_ON_ERROR(epd_turn_on_display());

    ESP_LOGI(TAG, "EPD clear done");

    return ESP_OK;
}

esp_err_t epd_clear_white(void)
{
    return epd_clear(EPD_COLOR_WHITE);
}

esp_err_t epd_clear_black(void)
{
    return epd_clear(EPD_COLOR_BLACK);
}

esp_err_t epd_sleep(void)
{
    ESP_LOGI(TAG, "EPD enter sleep");

    /*
     * 0x10: Deep Sleep Mode
     * 0x01: enter deep sleep
     */
    EPD_RETURN_ON_ERROR(epd_send_cmd(0x10));
    EPD_RETURN_ON_ERROR(epd_send_data(0x01));

    vTaskDelay(pdMS_TO_TICKS(100));

    return ESP_OK;
}

void epd_deinit(void)
{
    if (epd_spi != NULL) {
        spi_bus_remove_device(epd_spi);
        epd_spi = NULL;
    }

    spi_bus_free(EPD_HOST);
}