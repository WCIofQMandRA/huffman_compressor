//custream.hpp: 以编码单元为单位读写未压缩的文件
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <custream.hpp>
#include <iostream>
namespace hmcmpsr
{
std::unique_ptr<icustream> icustream::construct(unsigned culen,std::istream &is)
{
    if(culen==8)return std::make_unique<icustream_8>(is);
    else return std::make_unique<icustream_g>(culen,is);
}

std::unique_ptr<ocustream> ocustream::construct(unsigned culen,std::ostream &os)
{
    if(culen==8)return std::make_unique<ocustream_8>(os);
    else return std::make_unique<ocustream_g>(culen,os);
}

icustream_g::icustream_g(unsigned culen,std::istream &is):m_culen(culen),m_is(is){}

icustream& icustream_g::operator>>(uint64_t &ch)
{
    //current_char剩余的bit数足以填满ch
    if(current_char_bits>=m_culen)
    {
        ch=current_char&((1u<<m_culen)-1);
        current_char>>=m_culen;
        current_char_bits-=m_culen;
    }
    //current_char剩余的bit数无法填满ch
    else
    {
        unsigned i=current_char_bits;//ch被填充的bit数
        uint64_t j;//储存从m_is读入的字符
        //先将现有的bit填入ch
        ch=current_char;
        //读入若干字符，以填满ch
        while(i<m_culen)
        {
            j=m_is.get();
            if(j>=256)j=0;
            ch|=(j<<i);
            i+=8;
        }
        ch&=(1ull<<m_culen)-1;
        //把j中的bit填入ch后，还有多余的bit
        if(i>m_culen)
        {
            current_char=j>>(8-(i-m_culen));
            current_char_bits=i-m_culen;
        }
        //j中的bit恰好填满ch
        else
        {
            current_char=0;
            current_char_bits=0;
        }
    }
    return *this;
}

icustream_g::operator bool()
{
    return bool(m_is);
}

void icustream_g::clear()
{
    m_is.clear();
    current_char=0;
    current_char_bits=0;
}

ocustream_g::ocustream_g(unsigned culen,std::ostream &os):m_culen(culen),m_os(os){}

ocustream& ocustream_g::operator<<(uint64_t ch)
{
    //current_char中剩余的bit足以容纳ch
    if(m_culen+current_char_bits<8)
    {
        current_char|=uint8_t(ch<<current_char_bits);
        current_char_bits+=m_culen;
    }
    //current_char中剩余的bit无法容纳ch
    else
    {
        //用ch的低bit填满current_char，然后输出
        m_os.put(current_char|uint8_t(ch<<current_char_bits));
        unsigned i=m_culen-(8-current_char_bits);
        ch>>=(8-current_char_bits);
        //用ch中剩余bit填充整数个byte，并且输出
        while(i>=8)
        {
            m_os.put(uint8_t(ch));
            ch>>=8;
            i-=8;
        }
        //把ch中剩余的bit保存在current_char中
        current_char_bits=i;
        current_char=uint8_t(ch);
    }
    return *this;
}

void ocustream_g::sync()
{
    if(current_char_bits)
    {
        m_os.put(uint8_t(current_char));
        current_char=0;
        current_char_bits=0;
    }
}

icustream_8::icustream_8(std::istream &is):m_is(is){}
icustream_8::operator bool(){return bool(m_is);}
void icustream_8::clear()
{
    m_is.clear();
}

icustream& icustream_8::operator>>(uint64_t &ch)
{
    ch=(unsigned)m_is.get();
    if(ch>=256)ch=0;
    return *this;
}

ocustream_8::ocustream_8(std::ostream &os):m_os(os){}

ocustream& ocustream_8::operator<<(uint64_t ch)
{
    m_os.put(uint8_t(ch));
    return *this;
}

}
