//main.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
// This software is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#include <memory>
#include <fstream>
#include <iostream>
#include <hmcmpsr/hmcompressor.hpp>
#include "exit.hpp"
#include "argument.hpp"
#include <nowide/iostream.hpp>
#include <typeinfo>
#include <hmcmpsr/char_frequency.hpp>
#include <hmcmpsr/huffman_tree.hpp>
#include <iomanip>
#include <cmath>
#include "advancement_info.hpp"
#include <nowide/args.hpp>

int main(int argc,char **argv)
{
	nowide::args __args(argc,argv);
	try
	{
		catch_keyboardint();
		parse_argument(argc,argv);
		if(argu::raw_path)
		{
			if(argu::cmp_path)
			{
				nowide::cerr<<"不能同时压缩和提取\n";
				return 2;
			}
			hmcmpsr::single_cmpsr cmp;
			cmp.open(*argu::raw_path,argu::branch,argu::code_unit_length,
				(argu::branch==(argu::branch&-argu::branch))?argu::block_size:
				static_cast<uint64_t>(std::pow(argu::block_size,1.0/3.0)+1e-5));
			if(argu::show_frequency)
			{
				auto &cf=cmp.char_frequency();
				nowide::cout<<"│       字符        │       频次        │\n";
				for(auto i:cf)
				{
					nowide::cout<<"│"<<std::setw(19)<<i.first<<"│"<<std::setw(19)<<i.second<<"│\n";
				}
				nowide::cout<<std::flush;
				if(argu::exit_show_frequency)
					return 0;
			}
			if(argu::show_tree)
			{
				auto &tree=cmp.huffman_tree();
				nowide::cout<<tree<<std::endl;
				if(argu::exit_show_tree)
					return 0;
			}
			cmp.compress(argu::output_path?*argu::output_path:
				std::filesystem::u8path(argu::raw_path->u8string()+".hmz"),
				argu::override_existed);
			cmp.close();
		}
		else if(argu::cmp_path)
		{
			hmcmpsr::single_dcmpsr dcmp;
			dcmp.open(*argu::cmp_path);
			if(argu::show_head)
			{
				nowide::cout<<"原始文件大小: "<<dcmp.raw_file_size()<<"byte\n"
					"数据块数量:   "<<dcmp.data_blocks_number()<<"\n"
					"使用 "<<dcmp.huffman_tree_branches()<<"-叉Huffman树压缩\n"
					"字符长度:     "<<dcmp.code_unit_length()<<"bit"<<std::endl;
				if(argu::exit_show_head)
					return 0;
			}
			if(argu::show_tree)
			{
				auto &tree=dcmp.huffman_tree();
				nowide::cout<<tree<<std::endl;
				if(argu::exit_show_tree)
					return 0;
			}
			dcmp.decompress(argu::output_path?*argu::output_path:
				std::filesystem::u8path([](const std::string &s)
				{
					if(s.length()>=4&&s.substr(s.length()-4)==".hmz")
						return s.substr(0,s.length()-4);
					else throw std::invalid_argument("字符串 `"+s+"' 的后缀不是`.hmz'.");
				}(argu::cmp_path->u8string())),argu::override_existed);
			dcmp.close();
		}
		else
		{
			nowide::cerr<<"必须选择压缩或提取之一\n";
			return 2;
		}
		return 0;
	}
	catch(exit_type x)
	{
		return x.code;
	}
	catch(const std::exception& e)
	{
		//由于在GCC上获取人类可读的类型名比较麻烦，所以干脆全都用重整名。
		nowide::cerr<<"遇到异常 `"<<typeid(e).
#ifdef _MSC_VER
		raw_name()
#else
		name()
#endif
		<<"'\nwhat()= "<< e.what()<<'\n';
		return 2;
	}
}
