#include "driver/i2c.h"
#include "stdio.h"
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"

// 检测温湿度
#ifndef TEM_HUM.H
#define TEM_HUM .H
#include "tem_hum.h"
//E-Ink显示
#include "EPD_Test.h"
#include "ImageData.h"
#include "fonts.h"
#include "fonts.h"

//pwm空调控制
#include "red_pwm_control.h"

// MQTT连接
#include "mqtt.h"
#define DEFAULT_TASK_STACK_SIZE 0x1000
#define DEFAULT_TASK_PRIORITY 26
#define DELAYS_MS 1000

//

#endif

extern sensor_data_t data;
// 检测按键是否按下的中断回调函数
static int flag = 0;
static char remind[100]={0};
static int count=0;

static void gpio_callback_func(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    if (!flag) {
        flag = 1;
    }
    uapi_gpio_toggle(GPIO_01);
    osal_printk("button down!\n");
}

// 主函数
static void *Bedside_Assistant_task(void)
{ 
    //红外遥感控制空调的pwm驱动
     pwm_38khz_task();
    uapi_pin_init();
    uapi_gpio_init();
    uapi_pin_set_mode(GPIO_01, 0);
    uapi_gpio_set_dir(GPIO_01, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(GPIO_01, GPIO_LEVEL_HIGH);
   
    // 按键中断函数，避免"按键失灵"。
    uapi_gpio_register_isr_func(GPIO_00, GPIO_INTERRUPT_FALLING_EDGE, gpio_callback_func);

    // 初始化I2C的SCL与SDA
    uapi_pin_set_mode(15, 2);           // 复用 初始化 引脚15,16
    uapi_pin_set_mode(16, 2);           // 初始化 I2C
    uapi_i2c_master_init(1, 400000, 0); // 主机码：0

    // 检测温湿度数据
       SHT30_Read_Data(); 
    //MQTT连接
    wifi_connect();
    mqtt_connect();

    

    while (1) 
    {
      osDelay(DELAY_TIME_MS1); // 需要延时 否则会发布失败
        if (g_cmdFlag1) 
        {
            sprintf(g_send_buffer1, MQTT_CLIENT_RESPONSE1, g_response_id1);
            // 设备响应命令
            mqtt_publish1("v1/devices/me/telemetry", g_response_buf1); // topic.   msg
            g_cmdFlag1 = 0;
            memset(g_response_id1, 0, sizeof(g_response_id1) / sizeof(g_response_id1[0]));
        }
      
       
       
        // 温度大于25摄氏度，上报云平台提醒用户"注意中暑！"并自动调节温度
        if(data.temperature>25.0f)
        {
            //IR_Send(///);
            //温度23，模式制冷，风量自动
            IR_Send(0xB2, 0xBF, 0xD0);
            osDelay(DELAY_TIME_MS1);
            memset(remind, 0, sizeof(remind) / sizeof(remind[0]));
            sprintf(remind,"{\"Test\":\"温度为%.1f,注意中暑！\"}",data.temperature);
            mqtt_publish1("v1/devices/me/telemetry",remind);
           
        }
       
       
        
            // 属性上报部分
            osDelay(DELAY_TIME_MS1);
            memset(g_send_buffer1, 0, sizeof(g_send_buffer1) / sizeof(g_send_buffer1[0]));
            sprintf(g_send_buffer1, MQTT_DATA_SEND1,data.temperature ,data.humidity );
            mqtt_publish1("v1/devices/me/telemetry", g_send_buffer1);
            memset(g_send_buffer1, 0, sizeof(g_send_buffer1) / sizeof(g_send_buffer1[0]));
        
           
        // 按键按下就延时7秒，并进入中断回调函数
         if (flag) 
         {
            osal_mdelay(7);
            flag = 0;
         }

       
       

        //个性化提示
          if((data.temperature>20.0f&&data.temperature<26.0f)&&(data.humidity>40.0&&data.humidity<50.0))
        {
           // IR_Send(///);

            osDelay(DELAY_TIME_MS1);
            memset(remind, 0, sizeof(remind) / sizeof(remind[0]));
            sprintf(remind,"{\"Test\":\"气候很舒适,出去走几步吧!\"}");
            mqtt_publish1("v1/devices/me/telemetry",remind);
            //memset(remind, 0, sizeof(remind) / sizeof(remind));

        }
  
            //// 温湿度数据显示到屏幕
            EPD_DisplayTemperatureHumidity_Optimized(&data);

            // 增加刷新间隔，避免过于频繁刷新
            count++;
            if(count>=2000000)
            {
                osal_msleep(5000); // 30分钟刷新一次
                count=0;
            }
            
    }
    
    return NULL;
}

// 任务入口
static void task_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle =
        osal_kthread_create((osal_kthread_handler)Bedside_Assistant_task, NULL, "Bedside_Assistant_task", 0x1000);
    if (task_handle != NULL) 
    {
        osal_kthread_set_priority(task_handle, 26);
    } 
    else 
    {
        osal_printk("Failed to create SHT30 task\n");
    }
    osal_kthread_unlock();
}

app_run(task_entry);