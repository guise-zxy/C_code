#include "red_pwm_control.h"
uint8_t channel_id=3;
 uint8_t group_id=0;

//引导码L的对应pwm配置，修改点：.cycle=169
//
     pwm_config_t cfg_38khzL = {
        .low_time=1474,      // 低电平周期数 (70%)
        .high_time =632,     // 高电平周期数 (30%)
        .offset_time = 0,             // 分频系数
        .cycles =169,        // 发送周期数
        .repeat = false       // 重复模式，持续输出
    };


    //结束码的对应pwm配置，修改点：.cycle=169
     pwm_config_t cfg_38khzs = {
        .low_time=1474,      // 低电平周期数 (70%)
        .high_time =632,     // 高电平周期数 (30%)
        .offset_time = 0,             // 分频系数
        .cycles =169,        // 发送周期数
        .repeat = false       // 重复模式，持续输出
    };

    //数据0对应pwm配置，修改点：.cycles=21
    pwm_config_t cfg_38khz0 = { 
        .low_time=1474,      // 低电平周期数 (70%)
        .high_time =632,     // 高电平周期数 (30%)
        .offset_time = 0,             // 分频系数
        .cycles =21,        // 总周期数
        .repeat = false       // 重复模式，持续输出
    };

      pwm_config_t cfg_38khz1 = {
        .low_time=1474,      // 低电平周期数 (70%)
        .high_time =632,     // 高电平周期数 (30%)
        .offset_time = 0,             // 分频系数
        .cycles =21,        // 总周期数
        .repeat = false       // 重复模式，持续输出
    };



void Lead_Code(void)
{
    // 引导码 4400us 高电平, 4400us 低电平
    //通过分组启动pwm(CONFIG_PWM_GROUP_ID);
   /* 打开PWM通道 */
    errcode_t ret=uapi_pwm_open(channel_id, &cfg_38khzL);
    if(ret!=0)
    {
        osal_printk("Failed to open PWM channelL: %p\r\n", ret);
    }
    
    //设置分组uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
   
    //  uapi_pwm_set_group(group_id, &channel_id, 1);
     uapi_pwm_start_group(group_id);
    //  uapi_pwm_close(channel_id);
    osal_udelay(4400);
    osal_udelay(4400);

}

void Stop_Code(void)
{
  /* 打开PWM通道 */
    errcode_t ret=uapi_pwm_open(channel_id, &cfg_38khzs);
    if(ret!=0)
    {
        osal_printk("Failed to open PWM channelStop: %p\r\n", ret);
    }
    
    //设置分组uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
   
    // uapi_pwm_set_group(group_id, &channel_id, 1);
     uapi_pwm_start_group(group_id);
     osal_udelay(540);
   //uapi_pwm_close(channel_id);
     osal_udelay(5220);
}

void Send_0_Code(void)
{
    // 数据位 0，540us 高电平，540us 低电平
  /* 打开PWM通道 */
    errcode_t ret=uapi_pwm_open(channel_id, &cfg_38khz0);
    if(ret!=0)
    {
        osal_printk("Failed to open PWM channelL: %p\r\n", ret);
    }
    
    //设置分组uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
   
     //uapi_pwm_set_group(group_id, &channel_id, 1);
     uapi_pwm_start_group(group_id);
     osal_udelay(540);
     //uapi_pwm_close(channel_id);
     osal_udelay(540);
}

void Send_1_Code(void)
{ 
    /* 打开PWM通道 */
    errcode_t ret=uapi_pwm_open(channel_id, &cfg_38khz1);
    if(ret!=0)
    {
        osal_printk("Failed to open PWM channelL: %p\r\n", ret);
    }
    
    //设置分组uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
   
    //  uapi_pwm_set_group(group_id, &channel_id, 1);
     uapi_pwm_start_group(group_id);
     osal_udelay(540);
    //  uapi_pwm_close(channel_id);
     osal_udelay(1620);
}

// 发送字节码
void Send_Byte(uint8_t data)
{
    
    int i;
    for (i=7; i>=0; i--)
    {
        if (data & (1 << i))
        {
            Send_1_Code();
        }
        else
        {
            Send_0_Code();
        }
    }
}

void IR_Send(uint8_t A, uint8_t B, uint8_t C)
{
    // A 识别码 固定为 0xB2 (1011 0010)
    // B 风速
    // C 温度和模式
    
    Lead_Code();
    Send_Byte(A);
    Send_Byte(~A);
    Send_Byte(B);
    Send_Byte(~B);
    Send_Byte(C);
    Send_Byte(~C);
    Stop_Code();
    
    Lead_Code();
    Send_Byte(A);
    Send_Byte(~A);
    Send_Byte(B);
    Send_Byte(~B);
    Send_Byte(C);
    Send_Byte(~C);
    Stop_Code();

    osal_printk("elloooooo");
}

