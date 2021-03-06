//char_frequency.hpp: 每种字符出现的频率
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#pragma once
#ifdef HMCPSR_NOSTL
#   include <zzc_container/splay_tree.hpp>
#else
#   include <map>
#endif
#include <cstdint>

#ifdef HMCMPSR_DLLEXPORT
#   define HMCMPSR_API __declspec(dllexport)
#else
#	define HMCMPSR_API
#endif

namespace hmcmpsr
{
class icustream;
class HMCMPSR_API char_frequency_t: public
#ifdef HMCPSR_NOSTL
zzc::splay_tree
#else
std::map
#endif
<uint64_t,uint64_t>
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
