#ifndef IEC104_GATEWAY_H
#define IEC104_GATEWAY_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace IEC104 {

    const uint8_t START_BYTE = 0x68;
    const size_t APCI_LEN = 6;
    const size_t MAX_APDU_LEN = 253;

    // 控制域类型
    enum FrameFormat {
        FORMAT_I,   // 信息帧 (含数据)
        FORMAT_S,   // 监视帧 (确认)
        FORMAT_U,   // 控制帧 (连接管理)
        FORMAT_ERR  // 格式错误
    };

    // U帧功能码 (已验证位定义)
    enum UFunction : uint8_t {
        STARTDT_ACT = 0x07, // 0000 0111 (Bit0=1, Bit1=1, Bit2=1)
        STARTDT_CON = 0x0B, 
        STOPDT_ACT  = 0x13, 
        STOPDT_CON  = 0x23,
        TESTFR_ACT  = 0x43, 
        TESTFR_CON  = 0x83
    };

    // ASDU 类型标识 (仅列出项目常用)
    enum TypeID : uint8_t {
        // ---遥信(代表突发事件/开关变位)
        M_SP_NA_1=1, // 单点信息
        M_SP_TB_1=30,// 带CP56Time2a时标的单点信息
        // --- 遥测 (低优先级) ---
        M_ME_NA_1 = 9,   // 归一化测量值
        M_ME_TF_1 = 36,  // 带时标短浮点数 (关键)
        
        // --- 遥控 (高优先级) ---
        C_SC_NA_1 = 45,  // 单命令
        C_DC_NA_1 = 46,  // 双命令
        
        // --- 系统 ---
        C_IC_NA_1 = 100  // 总召唤
    };
}


// 解析结果包 (用于传递给 AI 或 ROHC)
struct IE104Packet {
    // 基础元数据
    bool valid = false;
    std::string error_msg;
    IEC104::FrameFormat format;
    
    // APCI 信息
    uint16_t tx_seq = 0;  // N(S)
    uint16_t rx_seq = 0;  // N(R)
    
    // ASDU 信息 (仅 I 帧有效)
    uint8_t type_id = 0;
    uint8_t sq_num = 0;       // 结构限定词
    uint16_t cot = 0;         // 传送原因
    uint16_t common_addr = 0; // 公共地址 (ROHC 字典Key)
    
    // 关键业务特征
    uint32_t first_ioa = 0;   // 首个信息对象地址
    int priority_level = 0;   // 0:普通, 1:高优, 2:低优
    
    // 原始载荷 (用于 AI 输入或透传)
    std::vector<uint8_t> payload; 
};

class IEC104Parser {
public:
    IEC104Parser();
    ~IEC104Parser();

    /**
     * @brief 解析入口函数
     * @param buffer 原始字节流
     * @param len 字节流长度
     * @return 解析出的结构化数据对象
     */
    IE104Packet parse(const uint8_t* buffer, size_t len);

    // 获取统计信息 
    std::string get_statistics() const;

private:
    // 内部解析助手
    void parse_apci(const uint8_t* buf, IE104Packet& pkt);
    void parse_asdu(const uint8_t* buf, size_t len, IE104Packet& pkt);
    
    // 辅助工具
    bool check_boundary(size_t current, size_t need, size_t total, std::string& err);

    // 统计计数器
    uint64_t total_packets_ = 0;
    uint64_t error_packets_ = 0;
    uint64_t i_frames_ = 0;
};

#endif // IEC104_GATEWAY_H