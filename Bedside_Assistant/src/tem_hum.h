

#include "driver/i2c.h"
#include "stdio.h"
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"

#define I2C_BUS_1        1  // 根据实际情况可能是 0 或其他，请参考 SDK
#define SHT30_ADDR       0x44 // SHT30 默认 I2C 地址



void SHT30_Read_Data(void);


