#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t audio_input_init(void);

/**
 * 读取 INMP441 音频，并转换为 16-bit PCM 单声道数据。
 *
 * out_buf: 输出缓冲区
 * sample_count: 希望读取的 PCM16 样本数
 * samples_read: 实际读取到的样本数
 */
esp_err_t audio_input_read_pcm16(int16_t *out_buf, size_t sample_count, size_t *samples_read);

/**
 * 读取一段音频并计算 RMS，主要用于调试麦克风是否有声音输入。
 */
esp_err_t audio_input_read_rms(int *out_rms);

#ifdef __cplusplus
}
#endif