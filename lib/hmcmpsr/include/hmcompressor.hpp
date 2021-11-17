//hmcompressor.hpp: Huffman树压缩器的高级封装
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
//单个文件的压缩器
class HMCMPSR_API single_cmpsr
{
public:
    void compress(std::ofstream &ofs,std::ifstream &ifs);
    unsigned n_branches=2;
    unsigned code_unit_length=8;
    //单位byte
    //实际的数据块大小是不超过这个数字的最大的code_unit_length/gcd(code_unit_length,8)的倍数
    uint64_t data_block_size=1048576/*1MiB*/;
};

class HMCMPSR_API single_dcmpsr
{
public:
    void decompress(std::ofstream &ofs,std::ifstream &ifs);
};
}

#undef HMCMPSR_API
