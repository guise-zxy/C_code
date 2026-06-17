#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"


#define EPD_WIDTH       400
#define EPD_HEIGHT      300
#define EPD_BUF_SIZE    (EPD_WIDTH * EPD_HEIGHT / 8)

#define EPD_COLOR_WHITE 0xFF
#define EPD_COLOR_BLACK 0x00

esp_err_t epd_gpio_init(void);
esp_err_t epd_spi_bus_init(void);
void epd_reset(void);

esp_err_t epd_send_cmd(uint8_t cmd);
esp_err_t epd_send_data(uint8_t data);
esp_err_t epd_send_data_buffer(const uint8_t *data, size_t len);

int epd_get_busy_level(void);
esp_err_t epd_wait_busy_timeout(uint32_t timeout_ms);

esp_err_t epd_init(void);
esp_err_t epd_display_frame(const uint8_t *frame_buffer);

esp_err_t epd_clear(uint8_t color);
esp_err_t epd_clear_white(void);
esp_err_t epd_clear_black(void);
esp_err_t epd_sleep(void);

void epd_deinit(void);

