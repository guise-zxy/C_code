#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <stdint.h>
#include <stdio.h>
#include "gpio.h"
#include "pinctrl.h"
#include "platform_core.h"
#include "spi.h"

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/**
 * e-Paper GPIO
**/
#define EPD_BUSY_PIN    GPIO_04 
#define EPD_RST_PIN     GPIO_05 
#define EPD_DC_PIN      GPIO_06 
#define EPD_CS_PIN      GPIO_08   
#define EPD_SCLK_PIN    GPIO_07  
#define EPD_MOSI_PIN    GPIO_09



#define SPI_MASTER_PIN_MODE 3
#define SPI_MASTER_BUS_ID 0

#define SPI_SLAVE_NUM                   1
#define SPI_FREQUENCY                   2
#define SPI_CLK_POLARITY                0
#define SPI_CLK_PHASE                   0
#define SPI_FRAME_FORMAT                0
#define SPI_FRAME_FORMAT_STANDARD       0

#define SPI_FRAME_SIZE_8                0x07  // 8位
#define SPI_TMOD                        0
#define SPI_WAIT_CYCLES                 0x10



void DEV_SPI_WriteByte(UBYTE value);
int DEV_Module_Init(void);
void DEV_Module_Exit(void);


#endif
