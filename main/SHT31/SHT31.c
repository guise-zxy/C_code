#include "SHT31.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "sht31";


//CRC 不是简单求和，而是一种按位推进、按规则异或多项式的校验。
static uint8_t sht31_crc8(const uint8_t *data, int len)
{
    uint8_t crc = 0xFF;

    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

//主控发送"测量命令"
static esp_err_t sht31_send_cmd(sht31_t *sht, uint16_t cmd)
{
    uint8_t buf[2] = {
        (uint8_t)(cmd >> 8),
        (uint8_t)(cmd & 0xFF)
    };

    return i2c_master_transmit(sht->dev_handle, buf, sizeof(buf), SHT31_TIMEOUT_MS);
}

esp_err_t sht31_probe(i2c_master_bus_handle_t bus_handle, uint8_t addr, uint32_t timeout_ms)
{
    return i2c_master_probe(bus_handle, addr, timeout_ms);
}

esp_err_t sht31_init(sht31_t *sht,
                     i2c_master_bus_handle_t bus_handle,
                     uint8_t addr,
                     uint32_t scl_speed_hz)
{
    if (sht == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(sht, 0, sizeof(*sht));
    sht->bus_handle = bus_handle;
    sht->addr = addr;

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = scl_speed_hz,
    };

    esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_config, &sht->dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }

//如果 SHT31 软复位失败，就把前面已经创建好的设备对象撤销掉，避免留下半成功、半失败的脏状态。
    ret = sht31_soft_reset(sht);
    if (ret != ESP_OK) {
        i2c_master_bus_rm_device(sht->dev_handle);
        sht->dev_handle = NULL;
        return ret;
    }

    ESP_LOGI(TAG, "SHT31 init ok, addr=0x%02X", sht->addr);
    return ESP_OK;
}

esp_err_t sht31_deinit(sht31_t *sht)
{
    if (sht == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (sht->dev_handle != NULL) {
        esp_err_t ret = i2c_master_bus_rm_device(sht->dev_handle);
        sht->dev_handle = NULL;
        return ret;
    }

    return ESP_OK;
}

esp_err_t sht31_soft_reset(sht31_t *sht)
{
    if (sht == NULL || sht->dev_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = sht31_send_cmd(sht, SHT31_CMD_SOFT_RESET);
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(5));
    return ESP_OK;
}

esp_err_t sht31_read_temp_humi(sht31_t *sht, float *temp_c, float *rh)
{
    if (sht == NULL || sht->dev_handle == NULL || temp_c == NULL || rh == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t raw[6] = {0};

    esp_err_t ret = sht31_send_cmd(sht, SHT31_CMD_MEAS_HIGHREP);
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(SHT31_MEAS_DELAY_MS));

    ret = i2c_master_receive(sht->dev_handle, raw, sizeof(raw), SHT31_TIMEOUT_MS);
    if (ret != ESP_OK) {
        return ret;
    }

    if (sht31_crc8(&raw[0], 2) != raw[2]) {
        ESP_LOGE(TAG, "temperature crc error");
        return ESP_FAIL;
    }

    if (sht31_crc8(&raw[3], 2) != raw[5]) {
        ESP_LOGE(TAG, "humidity crc error");
        return ESP_FAIL;
    }

    //SHT31 返回的温湿度数据是原始的 16 位整数，需要按照数据手册的公式转换成实际的物理值。
    //拼接成16bit整数，注意高字节在前，低字节在后
    uint16_t raw_t  = ((uint16_t)raw[0] << 8) | raw[1];
    uint16_t raw_rh = ((uint16_t)raw[3] << 8) | raw[4];

    *temp_c = -45.0f + 175.0f * ((float)raw_t / 65535.0f);
    *rh     = 100.0f * ((float)raw_rh / 65535.0f);

    //打印原始数据，方便调试CRC ；校验失败时，可以看看原始数据是不是读错了，还是 CRC 算错
    ESP_LOGI(TAG, "raw = %02X %02X %02X %02X %02X %02X",
             raw[0], raw[1], raw[2], raw[3], raw[4], raw[5]);

    return ESP_OK;
}