#ifndef _EPD_TEST_H_
#define _EPD_TEST_H_

#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include <stdlib.h> 

// 屏幕尺寸常量
#define EPD_WIDTH   200
#define EPD_HEIGHT  200

// 全局显示缓冲区
extern UBYTE *g_DisplayBuffer;
extern UWORD g_ImageSize;
typedef struct {
    float temperature;
    float humidity;
    uint8_t temp_unit; // 0: Celsius, 1: Fahrenheit
    uint8_t is_valid;  // 数据有效性标志
} sensor_data_t;





void DrawModernTitleBar(void);
void DrawTemperatureBox(sensor_data_t *data);
void DrawHumidityBox(sensor_data_t *data);


int EPD_DisplayTemperatureHumidity_Optimized(sensor_data_t *data);
void EPD_Cleanup(void);

#endif
