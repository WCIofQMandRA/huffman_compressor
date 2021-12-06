//hmcompressor.hpp: Huffman树压缩器的高层次封装
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#pragma once
#include <memory>
#include <filesystem>
#include <iostream>

#ifdef HMCMPSR_DLLEXPORT
#   define HMCMPSR_API __declspec(__dllexport__)
#else
#   define HMCMPSR_API
#endif

namespace hmcmpsr
{
class huffman_tree;
class char_frequency_t;
struct single_cmpsr_impl;
//单个文件的压缩器
class HMCMPSR_API single_cmpsr
{
public:
    single_cmpsr();
    ~single_cmpsr();
    //打开原始文件
    void open(const std::filesystem::path &raw_file_path,
        unsigned n_branches,
        unsigned code_unit_length,
        //实际的数据块大小是不超过这个数字的最大的code_unit_length/gcd(code_unit_length,8)的倍数
        uint64_t data_block_size=1048576/*1MiB*/);
    void close();
    //返回字符出现的频率
    const char_frequency_t& char_frequency();
    //返回压缩使用的Huffman树
    const class huffman_tree& huffman_tree();
    //压缩文件
    void compress(const std::filesystem::path &compressed_file_path,bool override_existed=false);
private:
    friend struct single_cmpsr_impl;
    std::unique_ptr<single_cmpsr_impl>m;
};

struct single_dcmpsr_impl;
class HMCMPSR_API single_dcmpsr
{
public:
    single_dcmpsr();
    ~single_dcmpsr();
    void open(const std::filesystem::path &compressed_file_path);
    void close();
    void decompress(const std::filesystem::path &uncompressed_file_path,bool override_existed=false);
    //查询原始文件大小
    uint64_t raw_file_size();
    //查询数据块的数量
    uint64_t data_blocks_number();
    //查询Huffman树是叉数
    unsigned huffman_tree_branches();
    //查询编码单元长度，单位bit
    unsigned code_unit_length();
    //查询Huffman树
    const class huffman_tree& huffman_tree();
private:
    friend struct single_dcmpsr_impl;
    std::unique_ptr<single_dcmpsr_impl> m;
};
}

#undef HMCMPSR_API
