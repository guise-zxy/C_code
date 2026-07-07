#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace OuterOHC {

constexpr uint8_t kMagicMask = 0xE0;
constexpr uint8_t kMagicValue = 0xE0;
constexpr uint8_t kTypeShift = 3;
constexpr uint8_t kTypeMask = 0x18;
constexpr uint8_t kFlagsMask = 0x07;

constexpr uint8_t kFlagFlowLabel = 0x01;
constexpr uint8_t kFlagHopLimit = 0x02;
constexpr uint8_t kFlagSegLeft = 0x04;
constexpr uint8_t kFlagFlowLabelShort = kFlagSegLeft;
constexpr uint8_t kFlagInnerType = kFlagHopLimit;

constexpr uint8_t kInnerTypeRohc = 0;
constexpr uint8_t kInnerTypeRaw = 1;

enum class Type : uint8_t {
    IR = 0,
    CO = 1,
};

inline uint8_t MakeHeader(Type type, uint8_t flags) {
    return static_cast<uint8_t>(kMagicValue |
                                ((static_cast<uint8_t>(type) << kTypeShift) & kTypeMask) |
                                (flags & kFlagsMask));
}

inline bool IsOhcPacket(uint8_t first_byte) {
    return (first_byte & kMagicMask) == kMagicValue;
}

} // namespace OuterOHC

enum class OuterCompressionMode {
    Raw,
    Ohc,
    Usid,
};

struct OuterOhcConfig {
    size_t refresh_interval = 200;
    size_t fallback_packets = 50;
    size_t error_window_packets = 100;
    double error_rate_threshold = 0.2;
    size_t error_min_samples = 20;
};

OuterCompressionMode ParseOuterCompressionMode(const std::string& value);

namespace OuterOhcFeedback {
constexpr char kMagic[4] = {'O', 'H', 'C', 'F'};
constexpr size_t kMsgSize = 8;
constexpr uint8_t kTypeRequestIr = 1;
constexpr uint8_t kTypeForceRaw = 2;
constexpr uint8_t kTypeRohcResync = 3;
constexpr uint8_t kTypeTestStop = 4;
} // namespace OuterOhcFeedback

class OuterOhcEncoder {
public:
    OuterOhcEncoder();

    void Configure(const OuterOhcConfig& cfg);
    void RequestIr(uint8_t cid);
    void ForceRaw();
    void RecordError();

    std::vector<uint8_t> Encode(const std::vector<uint8_t>& inner_payload,
                                const std::vector<std::string>& sids,
                                const std::string& src_ip,
                                const std::string& dst_ip,
                                uint32_t flow_label,
                                bool inner_is_raw);

private:
    struct Context {
        std::string src_ip;
        std::string dst_ip;
        std::vector<std::string> sids;
        size_t sent_since_ir = 0;
        bool force_ir = false;
    };

    std::string BuildKey(const std::string& src_ip,
                         const std::string& dst_ip,
                         const std::vector<std::string>& sids) const;

    uint8_t AllocateCidLocked(const std::string& key);

    std::mutex mtx_;
    std::unordered_map<std::string, uint8_t> key_to_cid_;
    std::unordered_map<uint8_t, std::string> cid_to_key_;
    std::unordered_map<uint8_t, Context> cid_to_ctx_;
    std::unordered_map<uint8_t, uint32_t> flowlabel_short_cache_;
    std::unordered_set<uint8_t> pending_ir_;
    uint8_t next_cid_ = 1;
    size_t refresh_interval_ = 200;
    size_t fallback_packets_ = 50;
    size_t force_raw_packets_ = 0;
    size_t error_window_packets_ = 100;
    double error_rate_threshold_ = 0.2;
    size_t error_min_samples_ = 20;
    size_t error_window_total_ = 0;
    size_t error_window_errors_ = 0;
};
