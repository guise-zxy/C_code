#include "tem_hum.h"

 //用于日志打印缓冲区
uint8_t a[6]={0};      

 //用于日志打印缓冲区
char  env[2000]={0};    

 //SHT30 测量命令：高重复性，拉伸使能
 uint8_t cmd_measure[] = {0x2C,0x06};      

//存储温湿度数据的信息结构体
typedef struct  hum_tem{
    float temperature;
    float humidity;
    uint8_t temp_unit; //
    uint8_t is_valid;  // 数据有效性标志： 0有效, 1 无效
} sensor_data_t;

//全局变量：用于后续存储及显示的温湿度缓冲区
sensor_data_t data={0};    


// 接收数据缓存区：6 字节（温度2 + CRC + 湿度2 + CRC）
uint16_t raw_temp = 0;
uint16_t raw_humi = 0;
float temperature = 0.0f;
float humidity = 0.0f;

// 用于 I2C 读写的数据结构体
//PS!!!!!!:接收数据与发送数据的缓冲区的指针不能为空
i2c_data_t write_data = {
    .send_buf = cmd_measure,
    .send_len = 2,
    
};

// 用于 I2C 读写的数据结构体
//PS!!!!!!:接收数据与发送数据的缓冲区的指针不能为空
i2c_data_t read_data = {

    .receive_buf = a,  // 动态分配 6 字节
    .receive_len = 6
};






// 读取 SHT30 数据并解析
void SHT30_Read_Data(void)
{
    // uapi_pin_set_mode(15,2);              //复用 初始化 引脚
    // uapi_pin_set_mode(16,2);                       // 初始化 I2C
    // uapi_i2c_master_init  ( 1,400000 ,0 );

    // 1. 发送测量命令
    int ret = uapi_i2c_master_write(I2C_BUS_1, SHT30_ADDR, &write_data);
    if (ret != 0) {
        osal_printk("SHT30 Write Failed: %x\n", ret);
      
    }

    osal_msleep(100); // 等待测量完成（根据 SHT30 datasheet，high repeatability 约 15ms，这里多等一点）

    // 2. 读取 6 字节数据
    ret = uapi_i2c_master_read(I2C_BUS_1, SHT30_ADDR, &read_data);
    if (ret != 0) {
        osal_printk("SHT30 Read Failed: %x\n", ret);
        return;
    }

    // 3. 解析数据
    uint8_t *buf = (uint16_t *)read_data.receive_buf;
    raw_temp = ((uint16_t)buf[0] << 8) | buf[1];  // 温度高低位
    raw_humi = ((uint16_t)buf[3] << 8) | buf[4];  // 湿度高低位

    // 4. 转换为实际物理值（根据 SHT30 数据手册公式）
    // 温度: T = -45 + 175 * (raw_temp / 65535.0)
    // 湿度: RH = 100 * (raw_humi / 65535.0)
    data.temperature = -45.0f + 175.0f * (raw_temp / 65535.0f);
    data.humidity = 100.0f * (raw_humi / 65535.0f);
    if(data.humidity<0||data.temperature<0)
    {
        data.is_valid = 1;
    }
    else{
        data.is_valid = 0;
    }
    // 5. 打印结果:若is_valid=0，则数据无效。串口打印
    if(data.is_valid==1)
    {
        osal_printk("data is invalid");
    }
    sprintf(env,"<:Temp: %.2f °C,<:Humi: %.2f %%RH\n",data.temperature,data.humidity);
    osal_printk("%s\n",env);
    // 6. 释放内存（如果每次都申请的话，也可以考虑静态分配）
    // 如果 read_data.receive_buf 是动态分配的，确保每次都是新申请或复用
}



