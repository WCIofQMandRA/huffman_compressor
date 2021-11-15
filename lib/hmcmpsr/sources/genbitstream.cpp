//genbitstream.cpp: 广义比特流
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <genbitstream.hpp>
#ifdef HMCPSR_NOGMP
#   include <mini_gmp.h>
#else
#   include <gmp.h>
#endif
#include <stdexcept>
#include <iostream>
#include <cstring>

namespace
{
//change endian: 在大端序时交换端序
#ifdef HMCMPSR_BIG_ENDIAN
template <typename IntType>
IntType ched(IntType x)
{
	char *px=reinterpret_cast<char*>(&x);
	static_assert(sizeof(x)<=8,"只支持64位及以下的整数");
	if constexpr(sizeof(x)==2)
	{
		char tmp=px[0];
		px[0]=px[1];
		px[1]=tmp;
	}
	else if constexpr(sizeof(x)==4)
	{
		char tmp0=px[0],tmp1=px[1];
		px[0]=px[3],px[1]=px[2];
		px[3]=tmp0,px[2]=tmp1;
	}
	else if constexpr(sizeof(x)==8)
	{
		char tmp0=px[0],tmp1=px[1],tmp2=px[2],tmp3=px[3];
		px[0]=px[7],px[1]=px[6],px[2]=px[5],px[3]=px[4];
		px[7]=tmp0,px[6]=tmp1,px[5]=tmp2,px[4]=tmp3;
	}
	return x;
}
#else
template<typename IntType>
IntType ched(IntType x){return x;}
#endif
}

#define M_BUFFER reinterpret_cast<__mpz_struct*>(m_buffer)

namespace hmcmpsr
{
ogenbitstream_n2::ogenbitstream_n2(unsigned radix):m_radix(radix)
{
    if(radix>=256)
        throw std::out_of_range("In hmcpsr::genobitstream_n2::genobitstream_n2, the radix is greater than 255.");
    m_buffer=new __mpz_struct;
    mpz_init_set_si(M_BUFFER,0);
}

ogenbitstream_n2::~ogenbitstream_n2()noexcept
{
    try
    {
        ::mpz_clear(M_BUFFER);
        delete M_BUFFER;
    }
    catch(...){}
}

void ogenbitstream_n2::putbit(unsigned bit)
{
    if(bit)
    {
        if(bit>=m_radix)
            throw std::out_of_range("In hmcpsr::ogenbitstream_n2::putbit, bit[which is"
            +std::to_string(bit)+"] >= m_radix[which is "+std::to_string(m_radix)+"].");
        mpz_t tmp;
        mpz_init(tmp);
        mpz_ui_pow_ui(tmp,m_radix,m_length);
        mpz_mul_ui(tmp,tmp,bit);
        mpz_add(M_BUFFER,M_BUFFER,tmp);
        mpz_clear(tmp);
    }
    ++m_length;
}

void ogenbitstream_n2::save(std::ostream &os)
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
    m_length=0;
}

igenbitstream_n2::igenbitstream_n2(unsigned radix):m_radix(radix)
{
    if(radix>=256)
        throw std::out_of_range("In hmcpsr::igenbitstream_n2::igenbitstream_n2, the radix is greater than 255.");
    m_buffer=new __mpz_struct;
    mpz_init(M_BUFFER);
}

igenbitstream_n2::~igenbitstream_n2()noexcept
{
    try
    {
        mpz_clear(M_BUFFER);
        delete M_BUFFER;
    }
    catch(...){}
}

unsigned igenbitstream_n2::getbit()
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

void igenbitstream_n2::load(std::istream &is)
{
    uint32_t compressed_genbits,compressed_bytes;
    is.read(reinterpret_cast<char*>(&compressed_genbits),4);
    is.read(reinterpret_cast<char*>(&compressed_bytes),4);
    m_length=ched(compressed_genbits);
    compressed_bytes=ched(compressed_bytes);
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

igenbitstream_n2::operator bool()
{
    return m_length!=0;
}
}
