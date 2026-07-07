#pragma once
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <memory>
#include <string>
#include <mutex>

// 前置声明，减少头文件依赖
namespace Proxy
{
    class ActiveClient;
}

class IEC104Client
{
public:
    enum class LinkState
    {
        DISCONNECTED,
        CONNECTED,
        ACTIVE
    };

    // 回调函数定义
    using OnConnectCallback = std::function<void()>;
    using OnActiveCallback = std::function<void()>;
    using OnDataCallback = std::function<void(uint8_t type_id, const std::vector<uint8_t> &asdu, const std::vector<uint8_t> &raw_pkt)>;

    IEC104Client(const std::string &ip, int port, const std::string &desc);
    ~IEC104Client();

    void Start();
    void Stop();
    bool IsActive() const;

    // 回调注册
    void SetOnConnectHandler(OnConnectCallback cb);
    void SetOnActiveHandler(OnActiveCallback cb);
    void SetOnDataHandler(OnDataCallback cb);

    // 业务接口
    void SendInterrogationCmd(uint16_t common_addr = 1);
    void SendSingleCommand(uint16_t common_addr, uint32_t ioa, bool on);
    void SendDoubleCommand(uint16_t common_addr, uint32_t ioa, bool close);
    void SendClockSyncCmd(uint16_t common_addr);

private:
    void HandlePacket(const std::vector<uint8_t> &data);
    void SendControlFrame(uint8_t type); // 发送 U帧 或 S帧
    void SendIFrame(const std::vector<uint8_t> &asdu);

private:
    std::shared_ptr<Proxy::ActiveClient> tcp_client_;
    std::string desc_;

    std::atomic<LinkState> state_{LinkState::DISCONNECTED};
    std::thread ticker_thread_;
    bool running_ = false;

    // 序列号管理
    // uint16_t send_seq_ = 0;
    // uint16_t recv_seq_ = 0;

    OnConnectCallback on_connect_;
    OnActiveCallback on_active_;
    OnDataCallback on_data_;

    // APCI 常量
    static const uint8_t U_STARTDT_ACT = 0x07;
    static const uint8_t U_STARTDT_CON = 0x0B;
    static const uint8_t U_TESTFR_ACT = 0x43;
    static const uint8_t U_TESTFR_CON = 0x83;

    std::atomic<uint16_t> send_seq_{0};
    std::atomic<uint16_t> recv_seq_{0};

    // 发送锁，防止I帧和S帧同时写入导致粘包/乱序
    std::mutex send_mutex_;
};