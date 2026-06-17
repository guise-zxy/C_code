#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 初始化 GPIO7 对应的 RMT 红外发射通道。
 */
esp_err_t ir_tx_init(void);

/**
 * 发送手机摄像头可观察的红外测试脉冲。
 */
esp_err_t ir_tx_send_camera_test(void);

/**
 * 发送美的 R05D 协议帧。
 */
esp_err_t ir_tx_send_midea_r05d(uint8_t a, uint8_t b, uint8_t c);

/**
 * 测试开机：
 * 制冷模式、自动风、25°C。
 */
esp_err_t ir_tx_send_midea_power_on_test(void);

/**
 * 测试关机。
 */
esp_err_t ir_tx_send_midea_power_off(void);

#ifdef __cplusplus
}
#endif