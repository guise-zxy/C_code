#include "DEV_Config.h"
#include "spi.h"
#include "gpio.h"
#include "pinctrl.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "errcode.h"
#include "soc_osal.h"


// extern SPI_HandleTypeDef hspi1;

#define SPI_TASK_PRIO                   24
#define SPI_TASK_STACK_SIZE             0x1000


void DEV_SPI_WriteByte(UBYTE value)//发送数据
{
    spi_xfer_data_t xfer = {0};
    xfer.tx_buff = &value;//设置缓冲区的一个地址
    xfer.tx_bytes = 1;
    xfer.rx_buff = NULL;
    xfer.rx_bytes = 0;
 // SPI 写单字节
  errcode_t ret = uapi_spi_master_write(SPI_MASTER_BUS_ID,&xfer, 100); 
  if (ret != ERRCODE_SUCC){
    osal_printk("failed,ret = 0x%x\n",ret);
  }
    // 如果没有 uapi_spi_write，可用：
    // spi_transmit_receive(SPI_MASTER_BUS_ID, &value, NULL, 1);

}


void app_spi_master_init_config(void)
{
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };

    config.is_slave = false;            //false意味WS63为主机
    config.slave_num = SPI_SLAVE_NUM;   //选择此时通信的从机编号
    config.bus_clk = 2000000;           //查看官方文档确认WS63的SPI总线的时钟源频率
    config.freq_mhz = SPI_FREQUENCY;    //期望的SPI通信频率（单位MHz） 控制器会根据 bus_clk计算分频值以接近此频率。
    config.clk_polarity = SPI_CLK_POLARITY;  //时钟极性：时钟线空闲时的电平高低
    config.clk_phase = SPI_CLK_PHASE;        //时钟相位：定义数据采样的时刻，比如从第几个边沿开始接收数据
    config.frame_format = SPI_FRAME_FORMAT;  //选择底层串行协议
    config.spi_frame_format = SPI_FRAME_FORMAT_STANDARD;//定义SPI的数据帧格式，例如数据位传输顺序是高位先行（MSB First）还是低位先行（LSB First）
    config.frame_size = SPI_FRAME_SIZE_8;  //SPI数据帧格式，此场景下是1字节
    config.tmod = SPI_TMOD;              //传输模式：设置数据传输是只读、只写还是全双工收发同时进行
    config.sste = 0;                     //从机选择切换使能：禁用时，主机读操作必须一次性读完从机数据，否则可能丢失数据

    ext_config.qspi_param.wait_cycles = SPI_WAIT_CYCLES;
    uapi_spi_init(SPI_MASTER_BUS_ID, &config, &ext_config);     //定义函数？
    
}

void spi_gpio_init(void)
{   
    uapi_gpio_init();             //调用函数？
    //uapi_pin_set_mode(CONFIG_SPI_DI_MASTER_PIN,3);
    uapi_pin_set_mode(EPD_MOSI_PIN,3);
    uapi_pin_set_mode(EPD_SCLK_PIN,3);
    uapi_pin_set_mode(EPD_CS_PIN,0);
    uapi_pin_set_mode(EPD_BUSY_PIN,2);
    uapi_pin_set_mode(EPD_RST_PIN,4);
    uapi_pin_set_mode(EPD_DC_PIN,0);

    
    uapi_pin_set_ds(EPD_SCLK_PIN, PIN_DS_7); // 由于时钟线的问题，需要设置驱动能力
    uapi_gpio_set_dir(EPD_BUSY_PIN, GPIO_DIRECTION_INPUT);
    uapi_gpio_set_dir(EPD_RST_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(EPD_DC_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(EPD_CS_PIN, GPIO_DIRECTION_OUTPUT);


    app_spi_master_init_config();
}

void DEV_Module_Exit(void)
{
    uapi_gpio_set_val(EPD_DC_PIN, 0);
    uapi_gpio_set_val(EPD_CS_PIN, 0);
    uapi_gpio_set_val(EPD_RST_PIN, 0);  
    uapi_spi_deinit(SPI_MASTER_BUS_ID);   
}

int DEV_Module_Init(void)
{
    //调用spi_gpio_init,若未成功则返回errcode_t;不为0即代表硬件初始化失败
    spi_gpio_init();
    //app_spi_master_init_config();   
    return 0;
}

