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
class HMCMPSR_API genbitsaver_base
{
public:
    virtual void putbit(unsigned bit)=0;//写入1bit信息
    virtual void save(std::ostream &os)=0;//将缓冲区的内容全部写入os
    virtual ~genbitsaver_base()noexcept =default;
};

class HMCMPSR_API genbitloader_base
{
public:
    virtual unsigned getbit()=0;//读取1bit信息
    virtual void load(std::istream &is)=0;//从is加载流
    virtual operator bool()=0;
    virtual ~genbitloader_base()noexcept =default;
};

//radix可以取任何值
class HMCMPSR_API genbitsaver_g:public genbitsaver_base
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
private:
    const unsigned m_radix;
    size_t m_length=0;
    void *m_buffer;
};

//radix可以取任何值
class HMCMPSR_API genbitloader_g:public genbitloader_base
{
public:
    genbitloader_g(unsigned radix);
    ~genbitloader_g()noexcept;
    operator bool()override;
    //读取1bit信息
    unsigned getbit()override;
    //从is加载流
    void load(std::istream &is)override;
private:
    const unsigned m_radix;
    size_t m_length=0;
    void *m_buffer;
};

//radix是2^n
class HMCMPSR_API genbitsaver_2n:public genbitsaver_base
{
public:
    //radix: 流的进制，n叉Huffman数要求radix=n
    //2<=n<=255
    genbitsaver_2n(unsigned radix);
    //写入1bit信息
    void putbit(unsigned bit)override;
    //将缓冲区的内容全部写入os
    void save(std::ostream &os)override;
private:
    const unsigned m_radix,log2_radix;
    size_t m_length=0;
    std::basic_string<unsigned char> m_buffer;
    unsigned char last_char=0;
    unsigned n_bits_left=0;
};

//radix是2^n
class HMCMPSR_API genbitloader_2n:public genbitloader_base
{
public:
    genbitloader_2n(unsigned radix);
    operator bool()override;
    //读取1bit信息
    unsigned getbit()override;
    //从is加载流
    void load(std::istream &is)override;
private:
    const unsigned m_radix,log2_radix;
    size_t m_length=0;
    std::basic_string<unsigned char> m_buffer;
    unsigned char first_char=0;
    unsigned n_bits_left=0;
    std::basic_string<unsigned char>::iterator input_iterator;
};

//辅助类
class genbitsaver
{
public:
    genbitsaver(unsigned radix)
    {
        if(radix==(radix&-radix))
            impl=std::make_unique<genbitsaver_2n>(radix);
        else
            impl=std::make_unique<genbitsaver_g>(radix);
    }
    void putbit(unsigned bit){impl->putbit(bit);}
    void save(std::ostream &os){impl->save(os);}
private:
    std::unique_ptr<genbitsaver_base> impl;
};

//辅助类
class genbitloader
{
public:
    genbitloader(unsigned radix)
    {
        if(radix==(radix&-radix))
            impl=std::make_unique<genbitloader_2n>(radix);
        else
            impl=std::make_unique<genbitloader_g>(radix);
    }
    unsigned getbit(){return impl->getbit();}
    void load(std::istream &is){impl->load(is);}
private:
    std::unique_ptr<genbitloader_base> impl;
};
}

#undef HMCMPSR_API
