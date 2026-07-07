#pragma once

#include "transport/io_interface.h"
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>

namespace Transport {

class SerialTransport : public IOInterface {
public:
    // 构造函数
    // device_path: 串口设备路径，如 "/dev/ttyUSB0"
    // baud_rate: 波特率，默认 2400 (DLT645标准)
    SerialTransport(const std::string& device_path, int baud_rate = 2400);
    ~SerialTransport();

    // --- IOInterface 接口实现 ---
    void Start() override;
    void Stop() override;
    bool Send(const std::vector<uint8_t>& data) override;
    
    void SetPacketHandler(PacketHandler handler) override {
        std::lock_guard<std::mutex> lock(cb_mutex_);
        packet_handler_ = handler;
    }
    
    void SetConnectHandler(ConnectHandler handler) override {
        std::lock_guard<std::mutex> lock(cb_mutex_);
        connect_handler_ = handler;
    }

    std::string GetDesc() const override {
        return "Serial(" + device_path_ + ")";
    }

private:
    // 打开并配置串口
    bool OpenSerial();
    // 关闭串口
    void CloseSerial();
    // 读线程主循环
    void ReadLoop();
    // 配置 Termios 参数
    bool ConfigureTermios(int fd, int baud_rate);

    std::string device_path_;
    int baud_rate_;
    int fd_; // 文件描述符

    std::atomic<bool> running_;
    std::thread read_thread_;
    
    PacketHandler packet_handler_;
    ConnectHandler connect_handler_;
    std::mutex cb_mutex_; // 保护回调函数
    std::mutex write_mutex_; // 保护写操作
};

}