//main.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
#include <char_frequency.hpp>
#include <huffman_tree.hpp>
#include <memory>
#include <fstream>
#include <iostream>
using namespace hmcmpsr;
using namespace std;

int main([[maybe_unused]]int argc,[[maybe_unused]]char **argv)
{
	char_frequency_t fre(8);
	ifstream fin("test.txt");
	fre.staticize(fin);
	huffman_tree tree;
	tree.build_tree(fre,2);
	cout<<tree<<endl;
	return 0;
}
