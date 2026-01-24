/*
 * Copyright (c) 2024 Beijing HuaQingYuanJian Education Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MQTT_H
 #define MQTT_H
#include "mqtt.h"
 #endif

volatile MQTTClient_deliveryToken deliveredToken;
 char g_send_buffer1[512] = {0};   // 发布数据缓冲区
char g_response_id1[100] = {0};  // 保存命令id缓冲区
//char g_send_buffer2[512]={0};
char g_response_buf1[] =
    "{\"Test\":\"SUCCESESS\"}"; // 响应json,保护响应返回的message

char * g_username1= "aiot_zxy";
char * g_password1= "194910@zhxy";
MQTTClient client;   //宏进行结构体初始化，避免未初始化字段的随机值问题
uint8_t g_cmdFlag1=0;

/* 回调函数，处理连接丢失 */
void connlost1(void *context, char *cause)
{
    unused(context);
    printf("Connection lost: %s\n", cause);
}

int mqtt_subscribe1(const char *topic)
{
    printf("subscribe start\r\n");
    MQTTClient_subscribe(client, topic, 1);
    return 0;
}


int mqtt_publish1(const char *topic, char *msg)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int ret = 0;
    pubmsg.payload = msg;
    pubmsg.payloadlen = (int)strlen(msg);
    pubmsg.qos = 1;
    pubmsg.retained = 0;
    printf("[payload]:  %s, [topic]: %s\r\n", msg, topic);
    ret = MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    osal_printk("\n%p",ret);
    if (ret != MQTTCLIENT_SUCCESS) {
        printf("mqtt publish failed\r\n");
        return ret;
    }

    return ret;
}

// 解析字符串并保存到数组中
void parse_after_slash1(const char *str, char *request_id)
{
    const char *last_slash = strchr(str, '/');
    if (last_slash!= NULL) {
        // 计算等\号后面的字符串长度
        strcpy(request_id, last_slash + 1);
    }
    else
    {
        request_id[0]='0';   //确保为空字符串
    }
}



/* 回调函数,处理publish的消息到达 */
void delivered1(void *context, MQTTClient_deliveryToken dt)
{
    unused(context);
    printf("Message with token value %d delivery confirmed\n", dt);

    deliveredToken = dt;
}

//服务端命令下发回调函数
int messageArrived1(void *context, char *topic_name, int topic_len, MQTTClient_message *message)
{
    unused(context);
    unused(topic_len);
    printf("[Message recv topic]: %s\n", topic_name);
    printf("[Message]: %s\n", (char *)message->payload);        // void *payload:指向消息实际内容
    if((char *)message->payload=="ON")
    {
          //温度26，模式制冷，风量自动
            IR_Send(10110010, 10111111,11010000);
   
    
    }
    else  if((char *)message->payload=="OFF")
    {
          //温度26，模式制冷，风量自动
            IR_Send(0xB2, 0x7B,0xED);
   
    
    }
    // // 进行传感器控制
    // if (strstr((char *)message->payload, "true") != NULL)
    //     my_io_setval(SENSOR_IO, GPIO_LEVEL_HIGH);
    // else
    //     my_io_setval(SENSOR_IO, GPIO_LEVEL_LOW);
    // 解析命令id
    parse_after_slash1(topic_name, g_response_id1);
    g_cmdFlag1 = 1;
    memset((char *)message->payload, 0, message->payloadlen);

    return 1;   // 表示消息已被处理
}

/*****************************************************************************
  STA 扫描-关联 sample用例
*****************************************************************************/
 errcode_t example_get_match_network(const char *expected_ssid,
                                           const char *key,
                                           wifi_sta_config_stru *expected_bss)// expected_bss:输出参数，用于存储找到的网络配置信息
{
    uint32_t num = WIFI_SCAN_AP_LIMIT; /* 64:扫描到的Wi-Fi网络数量 */
    uint32_t bss_index = 0;

    /* 获取扫描结果 */
    uint32_t scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);/*OSAL_GFP_ATOMIC:原子模式：SAL_GFP_ATOMIC - 使用原子分配，表示不允许睡眠（可能在中断上下文中使用）
这是嵌入式系统中常见的实时性要求体现*/
    if (result == NULL) {
        return ERRCODE_MALLOC;
    }

    memset_s(result, scan_len, 0, scan_len);   /*初始化：全部设置为0；防止缓冲区溢出
清零分配的内存，避免未初始化数据泄露*/
    if (wifi_sta_get_scan_info(result, &num) != ERRCODE_SUCC) {
        osal_kfree(result);//释放动态申请的内存空间
        return ERRCODE_FAIL;   //fail_fast原则
    }

    /* 筛选扫描到的Wi-Fi网络，选择待连接的网络 */
    /*60行比较：长度比较优先：快速排除长度不匹配的项，避免昂贵的字符串比较
      61行二进制比较：使用 memcmp 而非 strcmp，因为 SSID 可能包含非 ASCII 字符*/
    for (bss_index = 0; bss_index < num; bss_index++) {
        if (strlen(expected_ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(expected_ssid, result[bss_index].ssid, strlen(expected_ssid)) == 0) {
                break;//此时的bbs_index即为匹配的位置
                
            }
        }
    }

    /* 未找到待连接AP,可以继续尝试扫描或者退出，这里为直接退出 */
    if (bss_index >= num) {
        osal_kfree(result);
        return ERRCODE_FAIL;
          //fail_fast原则
    }
    /* 找到网络后复制网络信息和接入密码 */
    if (memcpy_s(expected_bss->ssid, WIFI_MAX_SSID_LEN/*33*/, result[bss_index].ssid, WIFI_MAX_SSID_LEN) != EOK) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    if (memcpy_s(expected_bss->bssid/*AP 的 MAC 地址（用于区分同名 AP）*/, WIFI_MAC_LEN, result[bss_index].bssid, WIFI_MAC_LEN) != EOK) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, WIFI_MAX_KEY_LEN, key, strlen(key)) != EOK/*0*/) {
        osal_kfree(result);
        return ERRCODE_MEMCPY;
    }
    expected_bss->ip_type = DHCP; /* IP类型为动态DHCP获取 */
    osal_kfree(result);
    return ERRCODE_SUCC;
}

errcode_t wifi_connect(void)
{
    char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; /* WiFi STA 网络设备名：是操作系统中用于标识网络接口的字符串名称，类似于 Linux 系统中的网络接口名称。 */
    wifi_sta_config_stru expected_bss = {0};         /* 连接请求信息 */
    const char expected_ssid[] = CONFIG_WIFI_SSID;
    const char key[] = CONFIG_WIFI_PWD; /* 待连接的网络接入密码 */
    struct netif *netif_p = NULL;
    wifi_linked_info_stru wifi_status;
    uint8_t index = 0;

    /*1. 等待wifi初始化完成 */
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); /* 10: 延时100ms  */
    }
    /*2.初始化设备的 Wi - Fi 为 STA 模式，并为后续“扫描热点、连接网络”做准备 */
    if (wifi_sta_enable() != ERRCODE_SUCC) {
        PRINT("STA enbale fail !\r\n");
        return ERRCODE_FAIL;
    }
    /*扫描并连接 WiFi*/
    do {
        PRINT("Start Scan !\r\n");
        (void)osDelay(100); /* 100: 延时1s  */
        /* 启动STA (站点模式，让主控板为"客户端"），扫描AP */
        if (wifi_sta_scan() != ERRCODE_SUCC) {
            PRINT("STA scan fail, try again !\r\n");
            continue;
        }

        (void)osDelay(300); /* 300: 延时100ms  */

        /* 获取待连接的网络AP */
        if (example_get_match_network(expected_ssid, key, &expected_bss) != ERRCODE_SUCC) {
            PRINT("Can not find AP, try again !\r\n");
            continue;
        }

        PRINT("STA start connect.\r\n");
        /* 启动连接 */
        if (wifi_sta_connect(&expected_bss) != ERRCODE_SUCC) {
            continue;
        }

        /* 检查网络是否连接成功 */
        for (index = 0; index < WIFI_CONN_STATUS_MAX_GET_TIMES; index++) {
            (void)osDelay(50); /* 50: 延时5s  */
            memset_s(&wifi_status, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));//初始化内存空间，以防数据泄露
            if (wifi_sta_get_ap_info(&wifi_status) != ERRCODE_SUCC) {
                continue;
            }
            if (wifi_status.conn_state == WIFI_CONNECTED) {
                break;
            }
        }
        if (wifi_status.conn_state == WIFI_CONNECTED) {
            break; /* 连接成功退出循环 */
        }
    } while (1);

    /*4. DHCP获取IP地址 */
    netif_p = netifapi_netif_find(ifname);     //通过给此函数传参数(WIFI-STA网络设备名:ifname)，获取网络设备结构体指针
    if (netif_p == NULL) {
        return ERRCODE_FAIL;
    }

    if (netifapi_dhcp_start(netif_p) != ERR_OK) {
        PRINT("STA DHCP Fail.\r\n");
        return ERRCODE_FAIL;
    }

    for (uint8_t i = 0; i < DHCP_BOUND_STATUS_MAX_GET_TIMES; i++) {
        (void)osDelay(50); /* 50: 延时500ms  */
        if (netifapi_dhcp_is_bound(netif_p) == ERR_OK) {
            PRINT("STA DHCP bound success.\r\n");
            break;
        }
    }

    //轮询检查是否获取到有效的 IP 地址
    for (uint8_t i = 0; i < WIFI_STA_IP_MAX_GET_TIMES; i++) {
        osDelay(1); /* 1: 延时10ms ；；；DHCP 配置 IP 需要时间，不能立即检查。延时给系统留出处理时间 */
        if (netif_p->ip_addr.u_addr.ip4.addr != 0) {             //IP 地址为 0.0.0.0（32 位全 0）表示未分配
            PRINT("STA IP %u.%u.%u.%u\r\n", (netif_p->ip_addr.u_addr.ip4.addr & 0x000000ff),
                  (netif_p->ip_addr.u_addr.ip4.addr & 0x0000ff00) >> 8,   /* 8: 移位  */
                  (netif_p->ip_addr.u_addr.ip4.addr & 0x00ff0000) >> 16,  /* 16: 移位  */
                  (netif_p->ip_addr.u_addr.ip4.addr & 0xff000000) >> 24); /* 24: 移位  */

            /* 连接成功 */
            PRINT("STA connect success.\r\n");
            return ERRCODE_SUCC;
        }
    }
    PRINT("STA connect fail.\r\n");
    return ERRCODE_FAIL;
}


 errcode_t mqtt_connect(void)
{
    int ret;
    //创建客户端配置信息的结构体
    MQTTClient_connectOptions conn_opts2 = MQTTClient_connectOptions_initializer;
    /* 初始化MQTT客户端 */
    MQTTClient_init();
    /* 创建 MQTT 客户端 */
    ret = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (ret != MQTTCLIENT_SUCCESS) {
        printf("Failed to create MQTT client, return code %d\n", ret);
        return ERRCODE_FAIL;
    }

    conn_opts2.keepAliveInterval = KeepAlive;
    conn_opts2.cleansession = 1;

    conn_opts2.username = g_username1;
    conn_opts2.password = g_password1;

    // 绑定回调函数
    MQTTClient_setCallbacks(client, NULL, connlost1, messageArrived1, delivered1);

    // 尝试连接
    if ((ret = MQTTClient_connect(client, &conn_opts2)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", ret);
        MQTTClient_destroy(&client); // 连接失败时销毁客户端
        return ERRCODE_FAIL;
    }
    printf("Connected to MQTT broker!\n");
    osDelay(DELAY_TIME_MS1);
    // 订阅MQTT主题
    mqtt_subscribe1(MQTT_CMDTOPIC_SUB1);
    
    return ERRCODE_SUCC;
}
