//custream.hpp: 以编码单元为单位读写未压缩的文件
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#pragma once
#include <cstdint>
#include <memory>
#include <iosfwd>

#ifdef HMCMPSR_DLLEXPORT
#   define HMCMPSR_API __declspec(__dllexport__)
#else
#   define HMCMPSR_API
#endif

namespace hmcmpsr
{
class HMCMPSR_API icustream
{
public:
    static std::unique_ptr<icustream> construct(unsigned culen,std::istream &is);
    virtual ~icustream()=default;
    virtual icustream& operator>>(uint64_t &ch)=0;
    virtual explicit operator bool()=0;
    virtual unsigned get_culen()const=0;
    virtual uint64_t tellg()=0;
    virtual void clear()=0;
};

class HMCMPSR_API ocustream
{
public:
    static std::unique_ptr<ocustream> construct(unsigned culen,std::ostream &os);
    virtual ~ocustream()=default;
    virtual ocustream& operator<<(uint64_t ch)=0;
    //同步会把最后一个不完整的字节的高位用0填充，然后写入os
    virtual void sync(){}
    virtual unsigned get_culen()const=0;
};

//icustream的通用的实现，1<=culen<=64
class HMCMPSR_API icustream_g: public icustream
{
public:
    icustream_g(unsigned culen,std::istream &is);
    icustream& operator>>(uint64_t &ch) override;
    explicit operator bool()override;
    unsigned get_culen()const override{return m_culen;}
    void clear()override;
    uint64_t tellg()override;
private:
    const unsigned m_culen;
    const uint64_t m_mask;
    std::istream &m_is;
    bool m_ok=true;
    uint8_t current_char=0;             //正在读入的字符
    unsigned current_char_bits=0;       //当前的字符已经读取了多少bit
                                        //assert(current_char_bits<8)
};

//ocustream的通用的实现，1<=culen<=64
class HMCMPSR_API ocustream_g: public ocustream
{
public:
    ocustream_g(unsigned culen,std::ostream &os);
    ocustream& operator<<(uint64_t ch)override;
    void sync()override;
    unsigned get_culen()const override{return m_culen;}
private:
    const unsigned m_culen;
    std::ostream &m_os;
    uint8_t current_char=0;            //正在输出的字符
    unsigned current_char_bits=0;      //当前的字符已经输出了多少bit
};

//针对culen==8的icustream实现
class HMCMPSR_API icustream_8: public icustream
{
public:
    icustream_8(std::istream &is);
    icustream& operator>>(uint64_t &ch) override;
    explicit operator bool()override;
    unsigned get_culen()const override{return 8;}
    void clear()override;
    uint64_t tellg()override;
private:
    std::istream &m_is;
};

//针对culen==8的ocustream实现
class HMCMPSR_API ocustream_8: public ocustream
{
public:
    ocustream_8(std::ostream &os);
    ocustream& operator<<(uint64_t ch) override;
    unsigned get_culen()const override{return 8;}
private:
    std::ostream &m_os;
};

//针对culen是8的因数的icustream实现
class HMCMPSR_API icustream_124: public icustream
{
public:
    icustream_124(unsigned culen,std::istream &is);
    icustream& operator>>(uint64_t &ch) override;
    explicit operator bool()override;
    unsigned get_culen()const override{return m_culen;}
    void clear()override;
    uint64_t tellg()override;
private:
    uint8_t current_char=0;
    unsigned current_char_bits=0;
    const unsigned m_culen;
    const uint8_t m_mask;
    std::istream &m_is;
};

//针对culen是8的因数的ocustream实现
class HMCMPSR_API ocustream_124: public ocustream
{
public:
    ocustream_124(unsigned culen,std::ostream &os);
    ocustream& operator<<(uint64_t ch) override;
    unsigned get_culen()const override{return m_culen;}
    void sync()override;
private:
    uint8_t current_char=0;
    unsigned current_char_bits=0;
    const unsigned m_culen;
    std::ostream &m_os;
};
}

#undef HMCMPSR_API
