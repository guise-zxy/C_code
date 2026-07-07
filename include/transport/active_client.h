#ifndef ACTIVE_CLIENT_H
#define ACTIVE_CLIENT_H

#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

namespace Proxy
{

    using PacketHandler = std::function<void(const std::vector<uint8_t> &payload)>;

    class ActiveClient
    {
    public:
        ActiveClient(std::string target_ip, int target_port, std::string protocol_name);
        ~ActiveClient();

        void Start();
        void Stop();
        void SetPacketHandler(PacketHandler handler);

        // 发送数据
        bool Send(const std::vector<uint8_t> &data);

        // 设置连接成功时的回调函数
        void SetConnectHandler(std::function<void()> handler)
        {
            connect_handler_ = handler;
        }

        std::string GetDesc() const
        {
            return name_;
        }

    private:
        std::string ip_;
        int port_;
        std::string name_;

        std::atomic<bool> running_;
        int sock_ = -1;

        PacketHandler handler_;
        std::mutex handler_mutex_;

        // 保存连接回调
        std::function<void()> connect_handler_;

        void WorkerLoop();
        bool Connect();
    };

}

#endif