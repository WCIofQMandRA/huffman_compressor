//main.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
#include <char_frequency.hpp>
#include <huffman_tree.hpp>
#include <genbitstream.hpp>
#include <memory>
#include <fstream>
#include <iostream>
using namespace hmcmpsr;
using namespace std;

int main([[maybe_unused]]int argc,[[maybe_unused]]char **argv)
{
	char_frequency_t fre(8);
	ogenbitstream bitout(2);
	igenbitstream bitin(2);
	ifstream fin("test.txt");
	ofstream fout("test.hmc");
	fre.staticize(fin);
	fin.clear();
	//fin.seekg(-1,ios::end);
	auto filesize=fin.tellg();
	cout<<filesize<<endl;
	fin.seekg(0);
	huffman_tree_base &tree=*(new huffman_tree);
	tree.build_tree(fre,2);
	tree.encode(bitout,fin,filesize);
	bitout.save(fout);
	fin.close();fout.close();

	fin.open("test.hmc");
	fout.open("test2.txt");
	bitin.load(fin);
	tree.decode(fout,bitin,-1);
	delete &tree;
	return 0;
}
