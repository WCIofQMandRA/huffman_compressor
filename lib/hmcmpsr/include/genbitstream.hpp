//genbitstream.hpp: 广义比特流，用于写README中的data_block_t
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#pragma once
#include <cstdint>
#include <iosfwd>
#ifdef HMCMPSR_DLLEXPORT
#   define HMCPSR_API __declspec(__dllexport__)
#else
#   define HMCPSR_API
#endif

namespace hmcmpsr
{
const unsigned GENBITSTREAM_EOF=256;

//广义bit流， 一般的bit只能取0/1，但为了处理n叉Huffman树，这里的bit可以取0/1/.../radix-1
class HMCPSR_API ogenbitstream_base
{
public:
    virtual void putbit(unsigned bit)=0;//写入1bit信息
    virtual void save(std::ostream &os)=0;//将缓冲区的内容全部写入os
    virtual ~ogenbitstream_base()noexcept =default;
};

class HMCPSR_API igenbitstream_base
{
public:
    virtual unsigned getbit()=0;//读取1bit信息
    virtual void load(std::istream &is)=0;//从is加载流
    virtual operator bool()=0;
    virtual ~igenbitstream_base()noexcept =default;
};

//radix不是2^n
class HMCPSR_API ogenbitstream_n2:public ogenbitstream_base
{
public:
    //radix: 流的进制，n叉Huffman数要求radix=n
    //2<=n<=255
    ogenbitstream_n2(unsigned radix);
    ~ogenbitstream_n2()noexcept;
    //写入1bit信息
    void putbit(unsigned bit)override;
    //将缓冲区的内容全部写入os
    void save(std::ostream &os)override;
private:
    const unsigned m_radix;
    size_t m_length=0;
    void *m_buffer;
};

//radix不是2^n
class HMCPSR_API igenbitstream_n2:public igenbitstream_base
{
public:
    igenbitstream_n2(unsigned radix);
    ~igenbitstream_n2()noexcept;
    operator bool();
    //读取1bit信息
    unsigned getbit()override;
    //从is加载流
    void load(std::istream &is)override;
private:
    const unsigned m_radix;
    size_t m_length=0;
    void *m_buffer;
};
}

#undef HMCPSR_API
