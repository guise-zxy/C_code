#ifndef TCP_PROXY_H
#define TCP_PROXY_H

#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <netinet/in.h>

namespace Proxy {

// 定义支持的协议类型 (用于日志和逻辑区分)
enum class ProtocolType {
    IEC_104,
    DLT_645,
    UNKNOWN
};

// 定义回调函数签名：收到数据后，把数据传出去
using PacketHandler = std::function<void(const std::vector<uint8_t>& payload)>;

class TcpProxy {
public:
    /**
     * @brief 构造函数
     * @param port 监听端口 (如 2404)
     * @param type 协议类型 (用于日志区分)
     * @param base_entropy 基础流熵 (用于区分不同的代理实例)
     */
    TcpProxy(int port, ProtocolType type, uint32_t base_entropy);

    // 析构函数：释放 Socket
    ~TcpProxy();

    // 启动监听线程 (非阻塞)
    void Start();

    // 停止服务
    void Stop();

    // 设置数据回调函数 (连接到 ROHC/SRv6 模块)
    void SetPacketHandler(PacketHandler handler);

private:
    int port_;
    ProtocolType type_;
    uint32_t base_entropy_;
    
    int server_sock_;
    std::atomic<bool> running_;
    
    PacketHandler handler_;
    std::mutex handler_mutex_; // 保护回调函数被多线程同时调用

    // 内部：接受连接循环
    void AcceptLoop();

    // 内部：处理单个客户端连接
    void HandleClient(int client_sock, std::string client_ip);

    // 辅助：设置 Socket 选项 (KeepAlive 等)
    void ConfigureSocket(int sock);
    
    // 辅助：获取协议名称字符串
    std::string GetProtocolName() const;
};

} // namespace Proxy

#endif // TCP_PROXY_H