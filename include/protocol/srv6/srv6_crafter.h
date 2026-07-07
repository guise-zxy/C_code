// #ifndef SRV6_CRAFTER_H
// #define SRV6_CRAFTER_H

// #include <vector>
// #include <string>
// #include <cstdint>

// // 定义命名空间，保持代码整洁
// namespace Protocol {

// class SRv6Crafter {
// public:
//     /**
//      * @brief 构造 G-SRv6 (uSID) 数据包
//      * * @param payload      业务载荷 (如 IEC104 APDU)
//      * @param usid_list    路径列表 (例如: {"2001:db8:1::", "2001:db8:2::"})
//      * @param src_ip       源 IPv6 地址 (网关虚拟 IP)
//      * @param dst_ip       目的 IPv6 地址 (云端入口 IP)
//      * @param flow_label   流标签 (用于辅助识别)
//      * @return std::vector<uint8_t> 序列化后的完整二进制包
//      */
//     static std::vector<uint8_t> CraftPacket(
//         const std::vector<uint8_t>& payload,
//         const std::vector<std::string>& usid_list,
//         const std::string& src_ip,
//         const std::string& dst_ip,
//         uint32_t flow_label = 0
//     );

// private:
//     // 内部辅助函数：手动构建 SRH 扩展头
//     static std::vector<uint8_t> BuildSRH(const std::vector<std::string>& usid_list);
// };

// } // namespace Protocol

// #endif // SRV6_CRAFTER_H


#ifndef SRV6_CRAFTER_H
#define SRV6_CRAFTER_H

#include <vector>
#include <string>
#include <cstdint>

namespace Protocol {

struct UsidConfig {
    std::string locator_prefix;
    size_t locator_bits = 48;
    size_t usid_bits = 16;
    size_t max_usids_per_sid = 6;
    bool strict_locator = true;
};

class SRv6Crafter {
public:
    /**
     * @brief 通用 SRv6 报文构造器
     * @param payload 原始应用层负载 (如 IEC104 ADPU)
     * @param sids    SRv6 路径列表 (支持任意跳数，为空则退化为 IPv6)
     * @param src_ip  源 IPv6
     * @param dst_ip  目的 IPv6 (通常是路径列表的第一个 SID，或者最终目的地)
     * @param flow_label 流标签 (包含 QoS 信息)
     * @param src_port 源端口 (用于构造 TCP 伪头，影响 ROHC 流区分)
     * @param dst_port 目的端口
     * @return std::vector<uint8_t> 完整的 IPv6 数据包
     */
    static std::vector<uint8_t> CraftPacket(
        const std::vector<uint8_t>& payload,
        const std::vector<std::string>& sids,
        const std::string& src_ip,
        const std::string& dst_ip,
        uint32_t flow_label,
        uint16_t src_port = 12345, // 默认值
        uint16_t dst_port = 80     // 默认值
    );
    static std::vector<uint8_t> CraftOuterPacket(
        const std::vector<uint8_t>& payload,
        const std::vector<std::string>& sids,
        const std::string& src_ip,
        const std::string& dst_ip,
        uint32_t flow_label,
        uint8_t next_header = 59
    );
    static std::vector<std::string> CompressUsidList(
        const std::vector<std::string>& sids,
        const UsidConfig& cfg
    );

};

}

#endif // SRV6_CRAFTER_H
