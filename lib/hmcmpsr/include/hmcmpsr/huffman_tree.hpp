//huffman_tree.hpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#pragma once
#include <memory>
#include <iosfwd>
#include <cstdint>
#ifdef HMCMPSR_DLLEXPORT
#   define HMCMPSR_API __declspec(dllexport)
#else
#	define HMCMPSR_API
#endif

namespace hmcmpsr
{
struct huffman_tree_impl;
class genbitsaver;
class genbitloader;
class icustream;
class ocustream;
class char_frequency_t;

class HMCMPSR_API huffman_tree
{
public:
    static std::unique_ptr<huffman_tree> construct(unsigned n_branches);
    virtual ~huffman_tree()=default;

    //根据字符串出现的频率建树, n_branches是Huffman树的叉数
    //取值范围：2<=n_branches<=256
    virtual void build_tree(const char_frequency_t &char_frequency,unsigned n_branches)=0;

    //从is读入n_chars个字符，将编码后的结果输出到os
    //os的radix必须与build_tree的n_branches相同
    virtual void encode(genbitsaver &os,icustream &is,size_t n_chars)=0;

    //从is读入若干字符，将解码后的结果输出到os，
    //is的radix必须与build_tree的n_branches相同
    virtual void decode(ocustream &os,genbitloader &is)=0;

    //从流读取Huffman树
    virtual void load_tree(std::istream &is)=0;
    virtual void save_tree(std::ostream &os)=0;//将Huffman树保存到流

    virtual unsigned get_code_unit_length()const=0;
    virtual unsigned get_n_branches()const=0;

    //输出Huffman树
    friend std::ostream& operator<<(std::ostream &os,const huffman_tree &tree)
    {
        tree.output_impl(os);
        return os;
    }
private:
    virtual void output_impl(std::ostream&)const=0;
};

class HMCMPSR_API huffman_tree_g: public huffman_tree
{
public:
    huffman_tree_g();
    ~huffman_tree_g();
    
    void build_tree(const char_frequency_t &char_frequency,unsigned n_branches)override;

    void encode(genbitsaver &os,icustream &is,size_t n_chars)override;
    void decode(ocustream &os,genbitloader &is)override;

    void load_tree(std::istream &is)override;
    void save_tree(std::ostream &os)override;

    unsigned get_code_unit_length()const override;
    unsigned get_n_branches()const override;
private:
    void output_impl(std::ostream&)const override;
    friend struct huffman_tree_impl;
    std::unique_ptr<huffman_tree_impl> m;
};
}
#undef HMCMPSR_API
