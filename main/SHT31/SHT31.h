#pragma once
/*
 * SHT31 minimal driver based on ESP-IDF i2c_master example
 * ESP-IDF new I2C master driver
 */

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c_master.h"


/* ===== SHT31 config ===== */
#define SHT31_ADDR_LOW              0x44
#define SHT31_ADDR_HIGH             0x45

/* single shot, high repeatability, clock stretching disabled */


#define SHT31_CMD_MEAS_HIGHREP   0x2400        //让 SHT31 做一次高重复度单次测量
#define SHT31_CMD_SOFT_RESET     0x30A2        //软复位：让 SHT31 自己恢复到干净状态

#define SHT31_TIMEOUT_MS         1000         //I2C 收发最多等多久，单位毫秒。
#define SHT31_MEAS_DELAY_MS      20          //发完测量命令后，先等多久再读数据


typedef struct {
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    uint8_t addr;
} sht31_t;


esp_err_t sht31_probe(i2c_master_bus_handle_t bus_handle, uint8_t addr, uint32_t timeout_ms);

esp_err_t sht31_init(sht31_t *sht,
                     i2c_master_bus_handle_t bus_handle,
                     uint8_t addr,
                     uint32_t scl_speed_hz);

esp_err_t sht31_deinit(sht31_t *sht);

esp_err_t sht31_soft_reset(sht31_t *sht);

esp_err_t sht31_read_temp_humi(sht31_t *sht, float *temp_c, float *rh);