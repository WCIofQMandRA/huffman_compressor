//char_frequency.cpp: 每种字符出现的频率
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <char_frequency.hpp>
#include <custream.hpp>
#include <iostream>

namespace hmcmpsr
{
void char_frequency_t::staticize(icustream &is)
{
    m_culen=is.get_culen();
    uint64_t current_char=0;
    while(true)
    {
        is>>current_char;
        if(!is)break;
        ++(*this)[current_char];
    }
}
}
