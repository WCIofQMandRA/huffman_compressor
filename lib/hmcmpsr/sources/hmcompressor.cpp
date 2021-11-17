//hmcompressor.cpp: Huffman树压缩器的高级封装
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <hmcompressor.hpp>
#include <genbitstream.hpp>
#include <custream.hpp>
#include <huffman_tree.hpp>
#include <char_frequency.hpp>
#include <fstream>
#include <numeric>

namespace hmcmpsr
{
void single_cmpsr::compress(std::ofstream &ofs,std::ifstream &ifs)
{
    //第一步: 统计字符频率，顺带获取文件长度
    char_frequency_t char_frequency;
    uint64_t raw_file_length,n_data_blocks,data_block_length;
    {
        uint64_t tmp=code_unit_length/std::gcd(code_unit_length,8);
        data_block_length=data_block_size/tmp*tmp;
    }
    {
        auto cuin=icustream::construct(code_unit_length,ifs);
        char_frequency.staticize(*cuin);
        ifs.clear();
        raw_file_length=ifs.tellg();
        ifs.seekg(0);
    }

    //第二步: 保存文件头
    n_data_blocks=(raw_file_length+data_block_length-1)/data_block_length;
    ofs.write(reinterpret_cast<char*>(&raw_file_length),8);
    ofs.write(reinterpret_cast<char*>(&n_data_blocks),8);

    //第三步：创建Huffman树
    auto hmtree=huffman_tree::construct(n_branches);
    hmtree->build_tree(char_frequency,n_branches);

    //第四步：保存Huffman树
    hmtree->save_tree(ofs);

    //第五步：压缩并保存压缩后的文件
    auto icus=icustream::construct(code_unit_length,ifs);
    for(uint64_t i=0;i<n_data_blocks;++i)
    {
        auto ogb=genbitsaver::construct(n_branches);
        hmtree->encode(*ogb,*icus,data_block_length);
        ogb->save(ofs);
    }
}

void single_dcmpsr::decompress(std::ofstream &ofs,std::ifstream &ifs)
{
    uint64_t raw_file_length,n_data_blocks;
    //第一步：读文件头
    ifs.read(reinterpret_cast<char*>(&raw_file_length),8);
    ifs.read(reinterpret_cast<char*>(&n_data_blocks),8);
    
    //第二步：读Huffman树
    unsigned n_branches=ifs.peek();
    auto hmtree=huffman_tree::construct(n_branches);
    hmtree->load_tree(ifs);

    //第三步：解压并保存原始文件
    auto ocus=ocustream::construct(hmtree->get_code_unit_length(),ofs);
    for(uint64_t i=0;i<n_data_blocks;++i)
    {
        auto igb=genbitloader::construct(n_branches);
        igb->load(ifs);
        hmtree->decode(*ocus,*igb);
    }
}
}
