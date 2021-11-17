//genbitstream.hpp: 广义比特流，用于写README中的data_block_t
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#pragma once
#include <cstdint>
#include <iosfwd>
#include <string>
#include <memory>
#ifdef HMCMPSR_DLLEXPORT
#   define HMCMPSR_API __declspec(__dllexport__)
#else
#   define HMCMPSR_API
#endif

namespace hmcmpsr
{
const unsigned GENBITSTREAM_EOF=256;

//广义bit流， 一般的bit只能取0/1，但为了处理n叉Huffman树，这里的bit可以取0/1/.../radix-1
class HMCMPSR_API genbitsaver
{
public:
    //创建合适的派生类
    static std::unique_ptr<genbitsaver> construct(unsigned radix);
    virtual void putbit(unsigned bit)=0;//写入1bit信息
    virtual void save(std::ostream &os)=0;//将缓冲区的内容全部写入os
    virtual ~genbitsaver()noexcept =default;
    virtual unsigned get_radix()const=0;
};

class HMCMPSR_API genbitloader
{
public:
    //创建合适的派生类
    static std::unique_ptr<genbitloader> construct(unsigned radix);
    virtual unsigned getbit()=0;//读取1bit信息
    virtual void load(std::istream &is)=0;//从is加载流
    virtual operator bool()=0;
    virtual ~genbitloader()noexcept =default;
    virtual unsigned get_radix()const=0;
};

//radix可以取任何值
class HMCMPSR_API genbitsaver_g:public genbitsaver
{
public:
    //radix: 流的进制，n叉Huffman数要求radix=n
    //2<=n<=256
    genbitsaver_g(unsigned radix);
    ~genbitsaver_g()noexcept;
    //写入1bit信息
    void putbit(unsigned bit)override;
    //将缓冲区的内容全部写入os
    void save(std::ostream &os)override;
    unsigned get_radix()const override{return m_radix;}
private:
    const unsigned m_radix;
    size_t m_length=0;
    void *m_buffer;
};

//radix可以取任何值
class HMCMPSR_API genbitloader_g:public genbitloader
{
public:
    genbitloader_g(unsigned radix);
    ~genbitloader_g()noexcept;
    operator bool()override;
    //读取1bit信息
    unsigned getbit()override;
    //从is加载流
    void load(std::istream &is)override;
    unsigned get_radix()const override{return m_radix;}
private:
    const unsigned m_radix;
    size_t m_length=0;
    void *m_buffer;
};

//radix是2^n
class HMCMPSR_API genbitsaver_2n:public genbitsaver
{
public:
    //radix: 流的进制，n叉Huffman数要求radix=n
    //2<=n<=255
    genbitsaver_2n(unsigned radix);
    //写入1bit信息
    void putbit(unsigned bit)override;
    //将缓冲区的内容全部写入os
    void save(std::ostream &os)override;
    unsigned get_radix()const override{return m_radix;}
private:
    const unsigned m_radix,log2_radix;
    size_t m_length=0;
    std::basic_string<unsigned char> m_buffer;
    unsigned char last_char=0;
    unsigned n_bits_left=0;
};

//radix是2^n
class HMCMPSR_API genbitloader_2n:public genbitloader
{
public:
    genbitloader_2n(unsigned radix);
    operator bool()override;
    //读取1bit信息
    unsigned getbit()override;
    //从is加载流
    void load(std::istream &is)override;
    unsigned get_radix()const override{return m_radix;}
private:
    const unsigned m_radix,log2_radix;
    size_t m_length=0;
    std::basic_string<unsigned char> m_buffer;
    unsigned char first_char=0;
    unsigned n_bits_left=0;
    std::basic_string<unsigned char>::iterator input_iterator;
};
}

#undef HMCMPSR_API
