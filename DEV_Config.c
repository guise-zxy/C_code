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
  errcode_t ret = uapi_spi_master_write(SPI_MASTER_BUS_ID,&xfer, 100); // WS63 SDK 通常提供此接口
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

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = 2000000;
    config.freq_mhz = SPI_FREQUENCY;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_FRAME_FORMAT;
    config.spi_frame_format = SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = SPI_FRAME_SIZE_8;
    config.tmod = SPI_TMOD;
    config.sste = 0;

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
    spi_gpio_init();
    //app_spi_master_init_config();   
    return 0;
}

