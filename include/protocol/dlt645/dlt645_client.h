#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>
#include "transport/io_interface.h"

class DLT645Client
{
public:
    using OnDataCallback = std::function<void(const std::vector<uint8_t> &raw_pkt)>;

    DLT645Client(std::shared_ptr<IOInterface> io,bool use_fe=true,int fe_count=4);
    ~DLT645Client();

    void Start();

    // 设置数据回调
    void SetOnDataHandler(OnDataCallback cb);

    // --- 业务接口 ---

    // 1. 读取通信地址
    void ReadAddress();

    // 2. 读取数据 (DI 需传入原始 4 字节，如 {0x00,0x01,0x00,0x00})
    void ReadData(const std::vector<uint8_t> &di);

    // 3. 拉闸/合闸控制
    void ControlSwitch(bool off);

    // 4. [高级] 修改密码 (生成攻击流量)
    void ModifyPassword(const std::vector<uint8_t> &old_pw, const std::vector<uint8_t> &new_pw);

    bool IsReady() const { return is_ready_; }

private:
    void HandlePacket(const std::vector<uint8_t> &data);
    void SendFrame(uint8_t ctrl_code, const std::vector<uint8_t> &data_body);

    std::shared_ptr<IOInterface> io_;
    OnDataCallback on_data_;

    // 电表地址 (默认为广播地址 AA AA AA AA AA AA)
    std::vector<uint8_t> meter_addr_;
    std::mutex send_mutex_;

    bool use_fe_prefix_ = true;
    int fe_count_ = 4;

    std::atomic<bool> is_ready_{false};
};