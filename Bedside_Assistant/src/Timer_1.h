#ifndef TIME1_H
#define TIME_H
#include "timer.h"
#include "tcxo.h"
#include "chip_core_irq.h"
#include "common_def.h"
#include "timer_porting.h"
#define TIMER_INDEX  1   //硬件定时器索引。
#define TIMER_PRIO 3    ///* 中断优先级范围，从高到低:  0~7 */

#define TIMER1_DELAY_US              // 定时器1：600000微秒 = 600000毫秒


#endif

extern errcode_t uapi_timer_init();                         // 1. 初始化定时器模块
extern  errcode_t  uapi_timer_adapter(timer_index_t index, uint32_t int_id, uint16_t int_priority);  // 2. 适配硬件定时器



typedef struct timer_info {
    uint32_t start_time;  // 定时器启动时间（毫秒）
    uint32_t end_time;    // 定时器超时时间（毫秒）
    uint32_t delay_time;  // 配置的延时时间（微秒）
} timer_info_t;

extern timer_info_t g_timers_info;