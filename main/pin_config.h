#pragma once

// EPD 已占用引脚，保留记录，避免后续冲突
#define EPD_PIN_MOSI   11
#define EPD_PIN_SCLK   12
#define EPD_PIN_CS     10
#define EPD_PIN_DC     13
#define EPD_PIN_RST    14
#define EPD_PIN_BUSY   21
#define EPD_PIN_MISO   -1

// SHT31 已占用引脚
#define I2C_MASTER_SDA_IO  8
#define I2C_MASTER_SCL_IO  9

// INMP441 I2S 引脚
#define INMP441_BCLK   4
#define INMP441_WS     5
#define INMP441_DIN    6

// 采样率：后续接 ESP-SR 时常用 16kHz
#define INMP441_SAMPLE_RATE_HZ  16000