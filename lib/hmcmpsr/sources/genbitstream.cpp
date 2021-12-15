//genbitstream.cpp: 广义比特流
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#include <hmcmpsr/genbitstream.hpp>
#ifdef HMCPSR_NOGMP
#   include <mini-gmp.h>
#else
#   include <gmp.h>
#endif
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstring>
#include "change_endian.hpp"

namespace
{
//因为2<=x<=256，所以从2^8开始判断。
constexpr unsigned log_2(unsigned x)
{
    unsigned y=0;
    if(x>=256)y+=4,x>>=8;
    if(x>=16)y+=4,x>>=4;
    if(x>=4)y+=2,x>>=2;
    if(x>=2)y+=1;
    return y;
}
}

#define M_BUFFER reinterpret_cast<__mpz_struct*>(m_buffer)
#define M_POW_RADIX_LENGTH reinterpret_cast<__mpz_struct*>(m_pow_radix_length)

namespace hmcmpsr
{

std::unique_ptr<genbitsaver> genbitsaver::construct(unsigned radix)
{
    if(radix==(radix&-radix))
        return std::make_unique<genbitsaver_2n>(radix);
    else
        return std::make_unique<genbitsaver_g>(radix);
}

std::unique_ptr<genbitloader> genbitloader::construct(unsigned radix)
{
    if(radix==(radix&-radix))
        return std::make_unique<genbitloader_2n>(radix);
    else
        return std::make_unique<genbitloader_g>(radix);
}

genbitsaver_g::genbitsaver_g(unsigned radix):m_radix(radix)
{
    if(radix>256)
        throw std::out_of_range("In hmcpsr::genbitsaver_g::genbitsaver_g, the radix is greater than 256.");
    m_buffer=new __mpz_struct;
    m_pow_radix_length=new __mpz_struct;
    mpz_init_set_ui(M_BUFFER,0);
    mpz_init_set_ui(M_POW_RADIX_LENGTH,1);
}

genbitsaver_g::~genbitsaver_g()noexcept
{
    try
    {
        ::mpz_clear(M_BUFFER);
        ::mpz_clear(M_POW_RADIX_LENGTH);
        delete M_BUFFER;
        delete M_POW_RADIX_LENGTH;
    }
    catch(...){}
}

void genbitsaver_g::putbit(unsigned bit)
{
    if(bit)
    {
        if(bit>=m_radix)
            throw std::out_of_range("In hmcpsr::genbitsaver_g::putbit, bit[which is"
            +std::to_string(bit)+"] >= m_radix[which is "+std::to_string(m_radix)+"].");
        mpz_t tmp;
        mpz_init(tmp);
        mpz_mul_ui(tmp,M_POW_RADIX_LENGTH,bit);
        mpz_add(M_BUFFER,M_BUFFER,tmp);
        mpz_clear(tmp);
    }
    mpz_mul_ui(M_POW_RADIX_LENGTH,M_POW_RADIX_LENGTH,m_radix);
    ++m_length;
}

void genbitsaver_g::save(std::ostream &os)
{
    //压缩后的理论长度，单位genbit，也就是压缩后的文件使用n进制输出时的长度
    uint32_t compressed_genbits=ched(static_cast<uint32_t>(m_length));
    //压缩后的实际长度，单位byte
    uint32_t compressed_bytes=ched(static_cast<uint32_t>(M_BUFFER->_mp_size
        *sizeof(mp_limb_t)));
    os.write(reinterpret_cast<char*>(&compressed_genbits),4);
    os.write(reinterpret_cast<char*>(&compressed_bytes),4);
    for(size_t i=0;i<M_BUFFER->_mp_size;++i)
    {
        mp_limb_t x=ched(M_BUFFER->_mp_d[i]);
        os.write(reinterpret_cast<char*>(&x),sizeof(x));
    }
    mpz_set_ui(M_BUFFER,0);
    mpz_set_ui(M_POW_RADIX_LENGTH,1);
    m_length=0;
}

genbitloader_g::genbitloader_g(unsigned radix):m_radix(radix)
{
    if(radix>256)
        throw std::out_of_range("In hmcpsr::genbitloader_g::genbitloader_g, the radix is greater than 256.");
    m_buffer=new __mpz_struct;
    mpz_init(M_BUFFER);
}

genbitloader_g::~genbitloader_g()noexcept
{
    try
    {
        mpz_clear(M_BUFFER);
        delete M_BUFFER;
    }
    catch(...){}
}

unsigned genbitloader_g::getbit()
{
    if(m_length)
    {
        mpz_t tmp;
        mpz_init(tmp);
        auto r=mpz_fdiv_qr_ui(M_BUFFER,tmp,M_BUFFER,m_radix);
        mpz_clear(tmp);
        --m_length;
        return static_cast<unsigned>(r);
    }
    else return GENBITSTREAM_EOF;
}

void genbitloader_g::load(std::istream &is)
{
    uint32_t compressed_genbits,compressed_bytes;
    is.read(reinterpret_cast<char*>(&compressed_genbits),4);
    is.read(reinterpret_cast<char*>(&compressed_bytes),4);
    m_length=ched(compressed_genbits);
    compressed_bytes=ched(compressed_bytes);
    //compressed_bytes过长很可能是压缩文件损坏导致的
    if(compressed_bytes>2147483648u)
        throw std::runtime_error("genbitloader_g::load: compressed_bytes is too long.");
    //如果压缩文件在32位系统上创建，并且在64位系统上读取，compressed_bytes可能不是sizeof(mp_limb_t)
    //的整数倍
    //向上取整用于防止缓冲区不足，向下取整用于防止从文件中读入过多的内容
    auto buf_sizec=(compressed_bytes+sizeof(mp_limb_t)-1)/sizeof(mp_limb_t),
        buf_sizef=(compressed_bytes)/sizeof(mp_limb_t);
    auto write_buf=mpz_limbs_write(M_BUFFER,buf_sizec);
    for(size_t i=0;i<buf_sizef;++i)
    {
        mp_limb_t x;
        is.read(reinterpret_cast<char*>(&x),sizeof(x));
        if(!is)
            throw std::runtime_error("genbitloader_g::load: Incomplete input stream.");
        write_buf[i]=ched(x);
    }
    if(buf_sizef<buf_sizec)
    {
        size_t n_bytes_left=compressed_bytes-buf_sizef*sizeof(mp_limb_t);
        char x[sizeof(mp_limb_t)];
        std::memset(x,0,sizeof(mp_limb_t));
        is.read(x,n_bytes_left);
        write_buf[buf_sizef]=ched(*reinterpret_cast<mp_limb_t*>(x));
    }
    mpz_limbs_finish(M_BUFFER,buf_sizec);
}

genbitloader_g::operator bool()
{
    return m_length!=0;
}

genbitsaver_2n::genbitsaver_2n(unsigned radix):m_radix(radix),log2_radix(log_2(radix))
{
    if(radix!=(radix&-radix))
        throw std::out_of_range("hmcpsr::genbitsaver_2n::genbitsaver_2n, radix is not 2^n.");
    if(radix>256)
        throw std::out_of_range("In hmcpsr::genbitsaver_2n::genbitsaver_2n, the radix is greater than 256.");
}

void genbitsaver_2n::putbit(unsigned bit)
{
    if(bit>=m_radix)
        throw std::out_of_range("In hmcpsr::genbitsaver_2n::putbit, bit[which is"
        +std::to_string(bit)+"] >= m_radix[which is "+std::to_string(m_radix)+"].");

    if(n_bits_left+log2_radix<=8)
    {
        last_char|=static_cast<uint8_t>(bit<<n_bits_left);
        n_bits_left+=log2_radix;
    }
    else
    {
        last_char|=static_cast<uint8_t>(bit<<n_bits_left);
        m_buffer.push_back(last_char);
        last_char=static_cast<uint8_t>(bit>>(8-n_bits_left));
        n_bits_left=(n_bits_left+log2_radix)-8;
    }
    ++m_length;
}

void genbitsaver_2n::save(std::ostream &os)
{
    if(n_bits_left)
        m_buffer.push_back(last_char);
    uint32_t compressed_genbits=ched(static_cast<uint32_t>(m_length));
    uint32_t compressed_bytes=ched(static_cast<uint32_t>(m_buffer.size()));
    os.write(reinterpret_cast<char*>(&compressed_genbits),4);
    os.write(reinterpret_cast<char*>(&compressed_bytes),4);
    os.write(reinterpret_cast<char*>(m_buffer.data()),m_buffer.size());
    m_buffer.clear();
    m_length=0,last_char=0,n_bits_left=0;
}

genbitloader_2n::genbitloader_2n(unsigned radix):m_radix(radix),log2_radix(log_2(radix))
{
    if(radix!=(radix&-radix))
        throw std::out_of_range("hmcpsr::genbitloader_2n::genbitloader_2n, radix is not 2^n.");
    if(radix>256)
        throw std::out_of_range("In hmcpsr::genbitloader_2n::genbitloader_2n, the radix is greater than 256.");
}

genbitloader_2n::operator bool()
{
    return m_length!=0;
}

unsigned genbitloader_2n::getbit()
{
    if(m_length==0)return GENBITSTREAM_EOF;
    if(n_bits_left>=log2_radix)
    {
        unsigned x=first_char&(m_radix-1u);
        first_char>>=log2_radix;
        n_bits_left-=log2_radix;
        --m_length;
        return x;
    }
    else
    {
        unsigned x=first_char;
        first_char=input_iterator==m_buffer.end()?0u:*input_iterator++;
        x|=first_char<<n_bits_left;
        first_char>>=log2_radix-n_bits_left;
        n_bits_left=8u-log2_radix+n_bits_left;
        --m_length;
        return x&=m_radix-1u;
    }
}

void genbitloader_2n::load(std::istream &is)
{
    unsigned compressed_genbits,compressed_bytes;
    is.read(reinterpret_cast<char*>(&compressed_genbits),4);
    is.read(reinterpret_cast<char*>(&compressed_bytes),4);
    m_length=ched(compressed_genbits);
    compressed_bytes=ched(compressed_bytes);
    //compressed_bytes过长很可能是压缩文件损坏导致的
    if(compressed_bytes>2147483648u)
        throw std::runtime_error("genbitloader_2n::load: compressed_bytes is too long.");
    m_buffer.resize(compressed_bytes);
    is.read(reinterpret_cast<char*>(m_buffer.data()),m_buffer.size());
    if(!is)
        throw std::runtime_error("genbitloader_2n::load: Incomplete input stream.");
    input_iterator=m_buffer.begin();
    first_char=0,n_bits_left=0;
}


}
