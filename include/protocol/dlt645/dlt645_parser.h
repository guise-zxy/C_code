#ifndef DLT645_PARSER_H
#define DLT645_PARSER_H

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

// 协议常量 [cite: 1870]
namespace DLT645 {
    const uint8_t START_BYTE = 0x68;
    const uint8_t END_BYTE   = 0x16;
    
    // 控制码定义 [cite: 1897-1906]
    enum ControlCode : uint8_t {
        CMD_READ_DATA       = 0x11, // 读数据
        CMD_READ_NEXT       = 0x12, // 读后续数据
        CMD_READ_ADDR       = 0x13, // 读地址
        CMD_WRITE_DATA      = 0x14, // 写数据 (高优)
        CMD_WRITE_ADDR      = 0x15, // 写地址 (高优)
        CMD_FREEZE          = 0x16, // 冻结
        CMD_CHANGE_RATE     = 0x17, // 改速率
        CMD_CLEAR_METER     = 0x1A, // 电表清零 (高优)
        CMD_CLEAR_EVENT     = 0x1B, // 事件清零 (高优)
        CMD_CONTROL_SWITCH  = 0x1C  // 拉合闸 (常用扩展，视为高优)
    };
}

// 解析结果结构体
struct DLT645Packet {
    bool valid = false;
    std::string error_msg;
    
    // 原始字段
    std::string address;    // 通信地址 (字符串形式，如 "000000000001")
    uint8_t ctrl_code = 0;  // 控制码
    uint8_t data_len = 0;   // 数据长度
    
    // 业务字段 (已减去 0x33 处理)
    std::vector<uint8_t> data_payload; 
    uint32_t data_id = 0;   // DI3-DI0 组合的数据标识
    
    // 优先级判断结果
    int priority_level = 0; // 0:低/普通, 1:高(控制/设置)
};

class DLT645Parser {
public:
    DLT645Parser();
    ~DLT645Parser();

    /**
     * @brief 解析单帧 DLT645 数据
     * @param buffer 输入缓冲区
     * @param len 长度
     * @return 解析结果
     */
    DLT645Packet parse(const uint8_t* buffer, size_t len);
};

#endif // DLT645_PARSER_H