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

/*
 * 从深睡眠中唤醒，仅做硬件复位，不发送 0x12（软件复位）。
 * 软件复位会清空显示 RAM，导致旧的显示内容丢失。
 * 用于局部刷新场景：epd_wake_from_sleep → epd_display_partial → epd_sleep。
 */
esp_err_t epd_wake_from_sleep(void);

esp_err_t epd_display_frame(const uint8_t *frame_buffer);

/*
 * 快速显示：使用 0xC7 波形，比 0xF7 少一次全屏清屏，闪烁更少。
 * 需要在 epd_init() 之后调用，init 中已预加载温度值。
 * 用于首次上电的全屏刷新，减少视觉冲击。
 */
esp_err_t epd_display_fast(const uint8_t *frame_buffer);

/* 局部刷新：只更新 0x24（黑白），不刷新 0x26（红色），减少闪烁 */
esp_err_t epd_display_partial(const uint8_t *frame_buffer);

esp_err_t epd_clear(uint8_t color);
esp_err_t epd_clear_white(void);
esp_err_t epd_clear_black(void);
esp_err_t epd_sleep(void);

void epd_deinit(void);

