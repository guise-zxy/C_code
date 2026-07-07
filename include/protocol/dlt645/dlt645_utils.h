#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace DLT645Utils
{
    inline void encode_data(std::vector<uint8_t>& frame,const std::vector<uint8_t>& raw)
    {
        for(uint8_t b:raw)
        {
            frame.push_back(b+0x33);
        }
    }

    //计算校验和
    inline uint8_t calc_cs(const std::vector<uint8_t>& frame,size_t start_idx)
    {
        uint8_t cs=0;
        for(size_t i=start_idx;i<frame.size();i++)
        {
            cs+=frame[i];
        }
        return cs;
    }

    // BCD转String 用于地址解析
    inline std::string  bcd_to_str_reverse(const std::vector<uint8_t>& bcd)
    {
        std::stringstream ss;
        for(auto it=bcd.rbegin();it!=bcd.rend();++it)
        {
            ss<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)*it;
        }
        return ss.str();
    }
}
