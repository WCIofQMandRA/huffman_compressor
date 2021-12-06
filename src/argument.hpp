//argument.hpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
// This software is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#pragma once
#include <optional>
#include <filesystem>
#include <cstdint>
#include <string>

namespace argu
{
extern std::string app_name;                                //可执行文件名
extern unsigned branch;                                     //Huffman树的叉数
extern unsigned code_unit_length;                           //编码单元的长度
extern uint64_t block_size;                                 //数据块的大小
extern std::optional <std::filesystem::path> output_path;   //输出文件的路径
extern std::optional <std::filesystem::path> raw_path;      //原始文件的路径
extern std::optional <std::filesystem::path> cmp_path;      //压缩文件的路径
extern bool show_tree;                                      //输出Huffman树
extern bool exit_show_tree;                                 //在输出Huffman树后退出
extern bool show_frequency;                                 //输出各个字符的出现频率
extern bool exit_show_frequency;                            //在输出字符频率后退出
extern bool show_head,exit_show_head;                       //输出文件头
extern bool override_existed;                               //覆盖已存在的文件
}

void parse_argument(int argc,char **argv);
