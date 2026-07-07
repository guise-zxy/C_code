#pragma once
#include "io_interface.h"
#include "transport/active_client.h" // 您原有的 TCP 客户端
#include <memory>

namespace Proxy
{

    class TcpTransport : public IOInterface
    {
    public:
        TcpTransport(const std::string &ip, int port, const std::string &desc)
        {
            client_ = std::make_shared<ActiveClient>(ip, port, desc);
        }

        void Start() override { client_->Start(); }
        void Stop() override { /* client_->Stop(); */ }

        bool Send(const std::vector<uint8_t> &data) override
        {
            return client_->Send(data);
        }

        void SetPacketHandler(PacketHandler handler) override
        {
            client_->SetPacketHandler(handler);
        }

        void SetConnectHandler(ConnectHandler handler) override
        {
            client_->SetConnectHandler(handler);
        }

        std::string GetDesc() const override { return client_->GetDesc(); }

    private:
        std::shared_ptr<ActiveClient> client_;
    };

}