/**
 * 输出38KHz PWM载波，30%占空比
 */
#include "common_def.h"
#include "pinctrl.h"
#include "pwm.h"
#include "tcxo.h"
#include "soc_osal.h"
#include "app_init.h"
extern  uint8_t channel_id;
extern  uint8_t group_id;

#define PWM_38KHZ_FREQ             38000       // 目标频率 38KHz
#define PWM_DUTY_CYCLE            30          // 占空比 30%
#define PWM_TASK_PRIO              24
#define PWM_TASK_STACK_SIZE        0x1000


 void * pwm_38khz_task();