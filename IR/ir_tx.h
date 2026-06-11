#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ir_tx_init(void);

/**
 * 发送一组便于手机摄像头观察的 38 kHz 红外测试闪烁。
 * 只用于验证 GPIO7、RMT 和红外发射模块是否正常工作。
 */
esp_err_t ir_tx_send_camera_test(void);

#ifdef __cplusplus
}
#endif