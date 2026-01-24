
#include "pwm_1.h"



/* PWM任务函数 */
void *pwm_38khz_task()
{
    //UNUSED(arg);
  
    /**
     * 配置38KHz载波，30%占空比
     * 假设PWM时钟为80MHz：
     */
    uapi_pwm_deinit();

    
    /* 配置引脚 */
    uapi_pin_set_mode(GPIO_11, 1);
    
//     /* 初始化PWM */
    uapi_pwm_init();


  //  uapi_pwm_set_group(group_id, &channel_id, 1);

    
    
//     /* 打开PWM通道 */
//     errcode_t ret=uapi_pwm_open(channel_id, &cfg_38khzL);
//     if(ret!=0)
//     {
//         osal_printk("Failed to open PWM channel: %p\r\n", ret);
//     }
    
//   //设置分组uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
   
 uapi_pwm_set_group(group_id, &channel_id, 1);


    
    osal_printk("SUCESSS to open  set channel");
    
  

// //     /* 启动PWM */
// #ifdef CONFIG_PWM_USING_V151
//     uint8_t channel_id = CONFIG_PWM_CHANNEL;
//     uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
//     uapi_pwm_start_group(CONFIG_PWM_GROUP_ID);
// #else
//     uapi_pwm_start(3);
// #endif

    //osal_printk("38KHz PWM载波已启动,占空比30%%\r\n");

    /* 任务持续运行，保持PWM输出 */
    // while (1) {
    //     uapi_tcxo_delay_ms(1000);  // 每秒输出一次状态
    //     osal_printk("PWM 38KHz running (30%% duty)...\r\n");
    // }

    // /* 正常不会到达这里 */
    // uapi_pwm_close(3);
    // uapi_pwm_deinit();
    
    return NULL;
}

// /* 创建任务 */
// static void pwm_38khz_entry(void)
// {
//     osal_task *task_handle = NULL;
//     osal_kthread_lock();
//     task_handle = osal_kthread_create((osal_kthread_handler)pwm_38khz_task, 
//                                        0, "Pwm38KHz", PWM_TASK_STACK_SIZE);
//     if (task_handle != NULL) {
//         osal_kthread_set_priority(task_handle, PWM_TASK_PRIO);
//     }
//     osal_kthread_unlock();
// }

///* 注册应用 */
// app_run(pwm_38khz_entry);
