#include "driver/spi.h"
#include "stdio.h"
#include "pinctrl.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"
#include "lwip/netifapi.h"
#include "wifi_hotspot.h"         //WiFi热点
#include "wifi_hotspot_config.h"  //WiFi热点配置
#include "stdlib.h"
#include "uart.h"
#include "lwip/nettool/misc.h"   //LwIP工具函数
#include "soc_osal.h"            //操作系统抽象层，提供任务和内存管理
#include "app_init.h"    
#include "cmsis_os2.h"           //CMSIS-RTOS2，提供任务和内存管理
#include "MQTTClientPersistence.h"
#include "MQTTClient.h"

#define CONFIG_WIFI_SSID "Excellent" // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD "200612zhxy" // 要连接的WiFi 热点密码



//#include "bsp/bsp.h"
#define WIFI_SCAN_AP_LIMIT 64
#define WIFI_CONN_STATUS_MAX_GET_TIMES 5 /* 启动连接之后，判断是否连接成功的最大尝试次数 */
#define DHCP_BOUND_STATUS_MAX_GET_TIMES 20 /* 启动DHCP Client端功能之后，判断是否绑定成功的最大尝试次数 */
#define WIFI_STA_IP_MAX_GET_TIMES 5 /* 判断是*/





//#include "bsp/bsp.h"
#define WIFI_SCAN_AP_LIMIT 64
#define WIFI_CONN_STATUS_MAX_GET_TIMES 5    /* 启动连接之后，判断是否连接成功的最大尝试次数 */
#define DHCP_BOUND_STATUS_MAX_GET_TIMES 20 /* 启动DHCP Client端功能之后，判断是否绑定成功的最大尝试次数 */
#define WIFI_STA_IP_MAX_GET_TIMES 5 /* 判断是否为有效IP的最大尝试次数 */


#define ADDRESS "thingskit.aiotcomm.com.cn:11883"
#define PORT    "11883"
#define CLIENTID "aiot_zxy"
#define MQTT_CMDTOPIC_SUB1 "v1/devices/me/rpc/request/+" // 设备订阅Topic(对于 云平台 就是publish下发命令)
#define MQTT_DATATOPIC_PUB1 "v1/devices/me/telemetry"                 // 属性上报topic
#define MQTT_CLIENT_RESPONSE1 "v1/devices/me/rpc/response/%s" // 命令响应topic



#define ZXY
#ifdef ZXY
extern char * g_username1;
extern char * g_password1;
#endif

#define MQTT_DATA_SEND1 "{\"Test\":\"Temp:% .1f||Humi:%.1f\"}"  // 上报数据格式



#define QOS 1
#define KeepAlive 120
#define DELAY_TIME_MS1 200

extern char g_send_buffer1[512];   // 发布数据缓冲区
extern char g_response_id1[100];  // 保存命令id缓冲区
extern char g_response_buf1[100];
//extern char g_send_buffer2[512];   // 发布数据缓冲区
    

extern MQTTClient client;   //宏进行结构体初始化，避免未初始化字段的随机值问题
extern uint8_t g_cmdFlag1;
extern int MQTTClient_init(void);




extern void connlost1(void *context, char *cause);


extern int mqtt_subscribe1(const char *topic);



extern int mqtt_publish1(const char *topic, char *msg);


// 解析字符串并保存到数组中
extern void parse_after_slash1(const char *str, char *request_id);




/* 回调函数,处理publish的消息到达 */
extern void delivered1(void *context, MQTTClient_deliveryToken dt);


//服务端命令下发回调函数
extern int messageArrived1(void *context, char *topic_name, int topic_len, MQTTClient_message *message);



extern errcode_t wifi_connect(void);
extern errcode_t mqtt_connect(void);
