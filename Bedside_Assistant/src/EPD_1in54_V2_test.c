#include "driver/i2c.h"
#include "stdio.h"
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"

// 检测温湿度
#include "tem_hum.h"

#include "EPD_Test.h"
#include "ImageData.h"
#include "fonts.h"
#include "fonts.h"
#include "EPD_1in54_V2.h"
// 对头文件里的extern的变量，完成定义
UBYTE *g_DisplayBuffer = NULL;
UWORD g_ImageSize = 0;

extern sensor_data_t data;

//获取到的温湿度数据缓冲区
char hum[20]={0};
char tem[20]={0};

/**
 * @brief 绘制现代化标题栏
 * 按照参数表规范：标题区(12,12,176,24)，分隔线Y=36
 */
void DrawModernTitleBar(void)
{
    // 1. 绘制标题文字（居中，Y=24是标题区中心）
    Paint_DrawString_EN(40, 18, "TEM AND HUM", &Font16, WHITE, BLACK);
    
    // 2. 绘制分隔线（标题下边框，Y=36，2px粗细）
    // 从X=12到X=12+176=188，长度176px
    Paint_DrawLine(12, 36, 188, 36, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(12, 37, 188, 37, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);  // 第二条线实现2px粗细
}

/**
 * @brief 绘制温度计图标
 * 严格按照README参数：16×16px，描边2.5px，中心点(100, 52)
 */





/**
 * @brief 绘制温度数据框
 * 严格按照README参数：温度区(12,36,176,70)，标签(100,52)，数值(100,75)
 */
void DrawTemperatureBox(sensor_data_t *data)
{

    char temp_label[20] = "Tem:";
    
    // === 1. 绘制温度图标（中心点：100, 52）===
    // README要求：图标16×16px，标签左侧，垂直居中
   sprintf(tem,"%.1f",data->temperature);
    
    // === 2. 绘制标签（README要求：10px，居中，坐标100, 52）===
    Paint_DrawString_EN(24, 65, temp_label, &Font24, WHITE, BLACK);
    
    // === 3. 绘制温度值（README要求：40px，居中，坐标100, 75）===
    Paint_DrawString_EN(95, 65, tem, &Font24, WHITE, BLACK);
    
    
    
}

/**
 * @brief 绘制湿度数据框
 * 严格按照README参数：湿度区(12,118,176,70)，标签(100,134)，数值(100,157)
 */
void DrawHumidityBox(sensor_data_t *data)
{
   
    char humi_label[20] = "Hum:";
    
    // 格式化湿度值（整数，符合README）
   sprintf(hum,"%.1f",data->humidity);
    
   
    
    // === 2. 绘制标签（README要求：10px，居中，坐标100, 134）===
    Paint_DrawString_EN(25, 160, humi_label, &Font24, WHITE, BLACK);
    
    // === 3. 绘制湿度值（README要求：40px，居中，坐标100, 157）===
    Paint_DrawString_EN(95, 165, hum, &Font24, WHITE, BLACK);
    
   
}

/**
 * @brief 绘制底部状态栏
 */
 
/**
 * @brief 显示温湿度数据到EPD屏幕（优化版）
 */
int EPD_DisplayTemperatureHumidity_Optimized(sensor_data_t *data)
{
    
    
    if (data == NULL || (data->is_valid)==1) {
        osal_printk("Invalid sensor data\r\n");
        return -1;
    }
    
    osal_printk("EPD Temperature/Humidity Display Start (Optimized)\r\n");
    
    // 初始化硬件模块
    DEV_Module_Init();
    osal_mdelay(500);
    
    // 初始化EPD显示屏
    osal_printk("e-Paper Init and Clear...\r\n");
    EPD_1IN54_V2_Init();
    EPD_1IN54_V2_Clear();
    osal_mdelay(500);
    
    // 分配显示缓冲区内存
    if (g_DisplayBuffer == NULL) {
        g_ImageSize = ((EPD_1IN54_V2_WIDTH % 8 == 0) ? 
                      (EPD_1IN54_V2_WIDTH / 8) : 
                      (EPD_1IN54_V2_WIDTH / 8 + 1)) * EPD_1IN54_V2_HEIGHT;
        
        g_DisplayBuffer = (UBYTE *)malloc(g_ImageSize);
        if (g_DisplayBuffer == NULL) {
            osal_printk("Failed to allocate display buffer memory\r\n");
            DEV_Module_Exit();
            return -1;
        }
    }
    
    // 创建新的图像
    Paint_NewImage(g_DisplayBuffer, EPD_1IN54_V2_WIDTH, EPD_1IN54_V2_HEIGHT, 0, WHITE);
    Paint_SelectImage(g_DisplayBuffer);
    Paint_Clear(WHITE);
    
    // 绘制各个界面元素
    DrawModernTitleBar();
    //绘制温度数据
    DrawTemperatureBox(data);
    //绘制湿度数据
    DrawHumidityBox(data);
    
    // 显示到屏幕
    EPD_1IN54_V2_Display(g_DisplayBuffer);
    osal_mdelay(300); // 等待显示稳定
    
    // 进入睡眠模式省电
    osal_printk("Entering sleep mode...\r\n");
    EPD_1IN54_V2_Sleep();
    
    osal_printk("Optimized display completed successfully\r\n");
    return 0;
}

/**
 * @brief 清理显示缓冲区
 */
void EPD_Cleanup(void)
{
    if (g_DisplayBuffer != NULL) {
        free(g_DisplayBuffer);
        g_DisplayBuffer = NULL;
    }
    g_ImageSize = 0;
}