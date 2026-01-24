#include "Timer_1.h"


static timer_info_t g_timers_info={0,0,TIMER1_DELAY_US };
/* 超时回调函数Timed task callback function list. */

static void timer_timeout_callback(uintptr_t data)
{
    uint32_t timer_index = (uint32_t)data;  // 获取定时器索引（0-3）
    g_timers_info.end_time = uapi_tcxo_get_ms();  // 记录结束时间
   
}
