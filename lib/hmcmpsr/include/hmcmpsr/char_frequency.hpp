//char_frequency.hpp: 每种字符出现的频率
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#pragma once
#include <map>
#include <cstdint>

#ifdef HMCMPSR_DLLEXPORT
#   define HMCMPSR_API __declspec(__dllexport__)
#else
#   define HMCMPSR_API
#endif

namespace hmcmpsr
{
class icustream;
class HMCMPSR_API char_frequency_t:public std::map<uint64_t,uint64_t>
{
public:
    //统计流中的字符的出现频次，给出filelength可方便输出进度信息
    void staticize(icustream &is,uint64_t filelength=0);
    //获取编码单元的长度
    unsigned get_code_unit_length()const {return m_culen;}
private:
    unsigned m_culen;
};
}

#undef HMCMPSR_API
