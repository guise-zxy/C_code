#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "protocol/dlt645/dlt645_parser.h"

namespace DLT645Handler {

    // 辅助函数：将 BCD 数据转为 double 用于显示 (处理 -0x33 后的数据)
    inline double bcd_to_double(const std::vector<uint8_t>& payload) {
        if (payload.empty()) return 0.0;
        std::stringstream ss;
        bool is_negative = false;

        // DLT645 的数据也是低位在前，高位在后
        // 且最高字节的最高位是符号位
        for (int i = payload.size() - 1; i >= 0; i--) {
            uint8_t val = payload[i];
            if (i == payload.size() - 1) {
                if (val & 0x80) { // 负数处理
                    is_negative = true;
                    val &= 0x7F;
                }
            }
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)val;
        }
        
        try {
            double val = std::stod(ss.str());
            // 通常电能/电压有小数位，这里默认除以100简化显示，实际需根据 DI 判断
            return is_negative ? -val / 100.0 : val / 100.0;
        } catch (...) {
            return 0.0;
        }
    }

    // 核心处理函数：被 main.cpp 调用
    inline DLT645Packet process_packet(const std::vector<uint8_t>& raw_data, DLT645Parser& parser) {
        // 1. 在原始流中寻找帧头 0x68
        // (因为 raw_data 可能包含 FE FE 前导符)
        size_t start_idx = 0;
        while(start_idx < raw_data.size() && raw_data[start_idx] != 0x68) {
            start_idx++;
        }

        if (start_idx >= raw_data.size()) return DLT645Packet{}; // 没找到帧头

        // 2. 截取从 0x68 开始的有效帧
        std::vector<uint8_t> frame(raw_data.begin() + start_idx, raw_data.end());

        // 3. 【关键】调用 Parser 进行解析
        // Parser 会进行校验和检查、长度检查、-0x33 解码等
        DLT645Packet pkt = parser.parse(frame.data(), frame.size());

        // 4. 根据解析结果打印日志
        if (pkt.valid) {
            // 控制码 0x91 是 "读数据响应" (0x11 | 0x80)
            if (pkt.ctrl_code == 0x91) {
                if (pkt.data_payload.size() > 8) {
                    // 长包（如事件记录）
                    std::cout << "[DLT645] Recv Large Block (Len: " << pkt.data_payload.size() << ") - [Event/History]" << std::endl;
                } else {
                    // 短包（数值）
                    // Payload 前4字节是 DI，后面是数据
                    if (pkt.data_payload.size() > 4) {
                        std::vector<uint8_t> val_bytes(pkt.data_payload.begin() + 4, pkt.data_payload.end());
                        double val = bcd_to_double(val_bytes);
                        std::cout << "[DLT645] Reading: " << std::fixed << std::setprecision(2) << val << std::endl;
                    }
                }
            } 
            // 控制码 0x93 是 "读地址响应"
            else if (pkt.ctrl_code == 0x93) {
                std::cout << "[DLT645] Meter Address Detected: " << pkt.address << std::endl;
            }
            // 控制码 0x9C 是 "拉闸/合闸响应"
            else if (pkt.ctrl_code == 0x9C) {
                std::cout << "[DLT645] Control Ack Received!" << std::endl;
            }
            // 错误应答 (0xD...)
            else if ((pkt.ctrl_code & 0xD0) == 0xD0) {
                 std::cout << "[DLT645] Error Response: " << std::hex << (int)pkt.ctrl_code << std::dec << std::endl;
            }
        } else {
            // 如果解析失败 (校验和错误等)，可选择打印调试信息
            // std::cout << "[DLT645] Invalid Frame: " << pkt.error_msg << std::endl;
        }
        return pkt;
    }
}