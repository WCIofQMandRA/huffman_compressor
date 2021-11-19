//main.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
#include <memory>
#include <fstream>
#include <iostream>
#include <hmcompressor.hpp>
#include "exit.hpp"
#include "argument.hpp"
#include <nowide/iostream.hpp>
#include <typeinfo>
#include <char_frequency.hpp>
#include <huffman_tree.hpp>
#include <iomanip>
#include <cmath>

int main(int argc,char **argv)
{
	try
	{
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
				std::filesystem::u8path(argu::raw_path->u8string()+".hmz"));
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
					else return s;
				}(argu::cmp_path->u8string())));
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
		nowide::cerr<<"遇到异常 `"<<typeid(e).name()<<"'\nwhat()= "<< e.what()<<'\n';
		return 2;
	}
}
