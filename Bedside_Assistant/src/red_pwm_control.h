
#include "pwm_1.h"
#include "pwm_porting.h"

#include "gpio.h"
#include "pwm.h"
#include "osal_task.h"
#include "pwm_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"


// #define DEFAULT_TASK_STACK_SIZE         0x1000
// #define DEFAULT_TASK_PRIORITY           26
// #define DELAYS_MS                       1000
// #define CONFIG_PWM_CHANNEL               3       // 配置使用的PWM通道
// #define TEST_MAX_TIMES                  10
// #define TEST_DELAY_MS                   1000
// #define CONFIG_PWM_GROUP_ID               0     /* 配置使用的PWM组*/
/* ========================================== */
/* R05D红外协议相关定义                      */
/* ========================================== */

/* R05D红外协议时序参数 (单位: 微秒) */
#define R05D_HEADER_HIGH_US             4400    /* 引导码高电平(载波) 9ms */
#define R05D_HEADER_LOW_US              4400    /* 引导码低电平 4.5ms */
#define R05D_BIT0_HIGH_US                540    /* 逻辑0高电平 560us */
#define R05D_BIT0_LOW_US                 540    /* 逻辑0低电平 560us */
#define R05D_BIT1_LOW_US                 540    /* 逻辑1高电平 1680us */
#define R05D_BIT1_HIGH_US               1620    /* 逻辑1低电平 1680us */
#define R05D_GAP_HIGH                  540    /* 帧间隔 20ms */
#define R05D_GAP_HIGH                  522    /* 帧间隔 20ms */
#define R05D_TRAILER_HIGH_US             540    /* 结束码高电平 */
#define R05D_TRAILER_LOW_US             5220    /* 结束码低电平 */

/* 载波参数: 38kHz */
#define R05D_CARRIER_FREQ              38000   /* 载波频率 38kHz */

#define TEST_MAX_TIMES 10
#define TEST_DELAY_MS 1000
#define DEFAULT_TASK_PRIORITY  26
#define DEFAULT_TASK_STACK_SIZE 0x1000


extern pwm_config_t cfg_38khzL;
extern pwm_config_t cfg_38khz1;
extern pwm_config_t cfg_38khz0;
extern pwm_config_t cfg_38khzs;


void Lead_Code(void);
void Stop_Code(void);
void Send_0_Code(void);
void Send_Byte(uint8_t);
void Send_1_Code(void);
void IR_Send(uint8_t A, uint8_t B, uint8_t C);


/* 1.美的空调控制开关编号 */



/* 2.美的空调控制MODE编号 */
typedef enum {
    AC_MODE_AUTO,
    AC_MODE_COOL,
    AC_MODE_DEHUMID,
    AC_MODE_HEAT,
    AC_MODE_FAN
} ac_mode_t;

/* 3.美的空调控制TEMP编号 */
typedef enum {
    AC_SPEED_AUTO,
    AC_SPEED_LOW,
    AC_SPEED_MED,
    AC_SPEED_HIGH
} ac_speed_t;


// case AC_MODE_AUTO:   return 0x08; /* 10 */
//         case AC_MODE_COOL:   return 0x00; /* 00 */
//         case AC_MODE_DEHUMID: return 0x04; /* 01 */
//         case AC_MODE_HEAT:   return 0x0C; /* 11 */
//         case AC_MODE_FAN:    return 0x04; /* 01 */
//         default:             return 0x00;