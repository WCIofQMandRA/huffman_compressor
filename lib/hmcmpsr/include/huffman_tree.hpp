//huffman_tree.hpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#pragma once
#include <memory>
#include <iosfwd>
#include <cstdint>
#ifdef HMCMPSR_DLLEXPORT
#   define HMCMPSR_API __declspec(__dllexport__)
#else
#   define HMCMPSR_API
#endif

namespace hmcmpsr
{
struct huffman_tree_impl;
class ogenbitstream_base;
class igenbitstream_base;
class char_frequency_t;
class HMCMPSR_API huffman_tree
{
public:
    huffman_tree();
    ~huffman_tree();
    //根据字符串出现的频率建树, n_branches是Huffman树的叉数
    //取值范围：2<=n_branches<=255
    void build_tree(const char_frequency_t &char_frequency,unsigned n_branches);

    //从is读入n_chars个字符，将编码后的结果输出到os, n_chars必须是编码单元大小的整数倍
    //若编码单元的大小不是整字节，n_chars还要是偶数
    //os的radix必须与build_tree的n_branches相同
    void encode(ogenbitstream_base &os,std::istream &is,size_t n_chars);

    //从is读入若干字符，将解码后的结果输出到os，使解码后的字符串的长度为n_chars, n_chars必须是符号单元大小的整数倍
    //若编码单元的大小不是整字节，n_chars还要是偶数
    //is的radix必须与build_tree的n_branches相同
    void decode(std::ostream &os,igenbitstream_base &is,size_t n_chars);

    //从流读取Huffman树
    void load_tree(std::istream &is,unsigned n_branches,unsigned code_unit_length);
    void save_tree(std::ostream &os);//将Huffman树保存到流
private:
    std::unique_ptr<huffman_tree_impl> m;
};
}
#undef HMCMPSR_API
