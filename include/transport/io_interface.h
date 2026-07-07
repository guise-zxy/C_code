#pragma once
#include <vector>
#include <functional>
#include <string>

using PacketHandler =std::function<void(const std::vector<uint8_t>&)>;
using ConnectHandler=std::function<void()>;

class IOInterface
{
public:
    virtual ~IOInterface()=default;

    virtual void Start()=0;
    virtual void Stop()=0;
    virtual bool Send(const std::vector<uint8_t>& data)=0;

    virtual void SetPacketHandler(PacketHandler handler) = 0;
    virtual void SetConnectHandler(ConnectHandler handler) = 0;
    
    // 获取描述 (用于日志)
    virtual std::string GetDesc() const = 0;
};