//custream.hpp: 以编码单元为单位读写未压缩的文件
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#include <hmcmpsr/custream.hpp>
#include <iostream>
namespace hmcmpsr
{
std::unique_ptr<icustream> icustream::construct(unsigned culen,std::istream &is)
{
    if(culen==8)return std::make_unique<icustream_8>(is);
    else if(8%culen==0)return std::make_unique<icustream_124>(culen,is);
    else return std::make_unique<icustream_g>(culen,is);
}

std::unique_ptr<ocustream> ocustream::construct(unsigned culen,std::ostream &os)
{
    if(culen==8)return std::make_unique<ocustream_8>(os);
    else if(8%culen==0)return std::make_unique<ocustream_124>(culen,os);
    else return std::make_unique<ocustream_g>(culen,os);
}

icustream_g::icustream_g(unsigned culen,std::istream &is):m_culen(culen),m_mask((1ull<<culen)-1),m_is(is)
{
    if(culen>64)
        throw std::out_of_range("In hmcpsr::icustream_g::icustream_g: culen>64");
}

icustream& icustream_g::operator>>(uint64_t &ch)
{
    bool is_first_get=true;
    //current_char剩余的bit数足以填满ch
    if(current_char_bits>=m_culen)
    {
        ch=current_char&m_mask;
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
            if(j>=256)
            {
                //第一次get就失败并且缓冲区中没有未读入的bits才返回错误
                if(is_first_get&&current_char_bits==0)
                    m_ok=false;
                j=0;
            }
            is_first_get=false;
            ch|=(j<<i);
            i+=8;
        }
        ch&=m_mask;
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
    return m_ok;
}

void icustream_g::clear()
{
    m_is.clear();
    current_char=0;
    current_char_bits=0;
    m_ok=true;
}

ocustream_g::ocustream_g(unsigned culen,std::ostream &os):m_culen(culen),m_os(os)
{
    if(culen>64)
        throw std::out_of_range("In hmcpsr::ocustream_g::ocustream_g: culen>64");
}

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
        m_os.put(current_char);
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

icustream_124::icustream_124(unsigned culen,std::istream &is):m_culen(culen),m_mask((1u<<culen)-1),m_is(is){}

icustream& icustream_124::operator>>(uint64_t &ch)
{
    if(!current_char_bits)
    {
        current_char=m_is.get();
        if(!m_is)current_char=0;
        current_char_bits=8;
    }
    ch=current_char&m_mask;
    current_char>>=m_culen;
    current_char_bits-=m_culen;
    return *this;
}

icustream_124::operator bool()
{
    return bool(m_is);
}

void icustream_124::clear()
{
    m_is.clear();
    current_char=0;
    current_char_bits=0;
}

ocustream_124::ocustream_124(unsigned culen,std::ostream &os):m_culen(culen),m_os(os){}

ocustream& ocustream_124::operator<<(uint64_t ch)
{
    current_char|=uint8_t(ch<<current_char_bits);
    current_char_bits+=m_culen;
    if(current_char_bits==8)
    {
        m_os.put(current_char);
        current_char=0;
        current_char_bits=0;
    }
    return *this;
}

void ocustream_124::sync()
{
    if(current_char_bits)
    {
        m_os.put(current_char);
        current_char=0;
        current_char_bits=0;
    }
}

uint64_t icustream_g::tellg(){return m_is.tellg();}
uint64_t icustream_8::tellg(){return m_is.tellg();}
uint64_t icustream_124::tellg(){return m_is.tellg();}

}
