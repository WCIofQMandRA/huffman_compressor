//char_frequency.cpp: 每种字符出现的频率
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <char_frequency.hpp>
#include <iostream>

namespace hmcmpsr
{
void char_frequency_t::staticize(std::istream &is)
{
    uint64_t current_char=0;
    //当前的字符已经读取了多少bit
    unsigned current_char_bits=0;
    while(is)
    {
        unsigned chi=(unsigned)is.get();
        if(chi>=256)break;
        uint8_t ch=chi;
        if(current_char_bits+8<m_culen)
        {
            current_char|=(uint64_t(ch)<<current_char_bits);
            current_char_bits+=8;
        }
        else
        {
            uint64_t x=current_char|(uint64_t(ch)<<current_char_bits);
            if(m_culen!=64)x&=(1ull<<m_culen)-1;
            ++(*this)[x];
            current_char=ch>>(m_culen-current_char_bits);
            current_char_bits=current_char_bits+8-m_culen;
            //在m_culen<8时，读入1byte的信息可能填满多个编码单元
            while(current_char_bits>=m_culen)
            {
                x=current_char&((1ull<<m_culen)-1);
                ++(*this)[x];
                current_char>>=m_culen;
                current_char_bits-=m_culen;
            }
        }
    }
    if(current_char_bits)
        ++(*this)[current_char];
}
}
