#ifndef ROHC_MANAGER_H
#define ROHC_MANAGER_H

#include <vector>
#include <array>
#include <cstddef>
#include <cstdint>
#include <arpa/inet.h>
#include <string>
#include <atomic>
#include <functional>
#include "rohc/rohc.h"      
#include "rohc/rohc_comp.h"
#include "utils/policy_types.h"
#include <mutex>
#include <thread>

enum class TunnelMode {
    Udp,
    L2,
};

struct TunnelConfig {
    TunnelMode mode = TunnelMode::Udp;
    std::string l2_iface;
    std::string l2_dst_mac;
    uint16_t l2_ethertype = 0x88B5;
};

class ROHCManager {
private:
    struct rohc_comp *compressor = nullptr;
    uint8_t output_buffer[2048]; 
    uint32_t tcp_seq_num = 1000;  //模拟tcp序列号

    int tunnel_sock=-1;
    struct sockaddr_in cloud_addr;
    bool tunnel_ready=false;
    std::mutex comp_mutex_;
    RohcPolicy last_policy_;
    bool policy_inited_ = false;


public:
    ROHCManager();
    ~ROHCManager();

    using FeedbackHandler = std::function<void(uint8_t type, uint8_t cid)>;

    //初始化UDP隧道目标地址
    void init_tunnel(const std::string& cloud_ip,int cloud_port);
    void SetFeedbackHandler(FeedbackHandler handler);

    // 核心功能：压缩真实流量
    void compress_real_traffic(const std::vector<uint8_t>& payload, const char* tag, uint32_t flow_label);

    CompressionStats compress_inner(const std::vector<uint8_t> &full_packet, const RohcPolicy& policy);
    void ConfigureRecovery(bool enable, size_t fallback_packets);
    void ConfigureCid(const std::string& cid_type, size_t max_cid);
    void ConfigureTunnelHeader(bool enable_seq_ts);
    void ConfigureTunnelTransport(const TunnelConfig& cfg);
    void TriggerRecovery();
    bool send_tunnel_payload(const std::vector<uint8_t> &payload, uint64_t send_ts_us);
    bool TestStopRequested() const;

private:
    bool InitCompressor(rohc_cid_type_t cid_type, size_t max_cid);
    void StartFeedbackListener();
    void FeedbackLoop();
    void InitTestControlFromEnvLocked();

    FeedbackHandler feedback_handler_;
    std::mutex feedback_mtx_;
    std::atomic<bool> feedback_running_{false};
    std::thread feedback_thread_;
    bool recovery_enabled_ = false;
    size_t recovery_fallback_packets_ = 0;
    size_t force_raw_packets_ = 0;
    rohc_cid_type_t cid_type_ = ROHC_SMALL_CID;
    size_t max_cid_ = ROHC_SMALL_CID_MAX;
    bool tunnel_seq_ts_enable_ = true;
    TunnelMode tunnel_mode_ = TunnelMode::Udp;
    std::string l2_iface_;
    std::string l2_dst_mac_text_;
    uint16_t l2_ethertype_ = 0x88B5;
    int l2_ifindex_ = 0;
    std::array<uint8_t, 6> l2_dst_mac_{};
    std::array<uint8_t, 6> l2_src_mac_{};
    bool l2_dst_mac_set_ = false;
    bool test_control_inited_ = false;
    bool test_control_enabled_ = false;
    bool test_stop_feedback_enable_ = false;
    uint64_t test_target_packets_ = 0;
    uint64_t test_sent_packets_ = 0;
    std::atomic<bool> test_stop_requested_{false};
    std::atomic<uint64_t> test_stop_start_ms_{0};
    std::atomic<uint64_t> test_stop_grace_ms_{0};
};

#endif // ROHC_MANAGER_H
