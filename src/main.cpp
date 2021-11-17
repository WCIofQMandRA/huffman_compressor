//main.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
#include <char_frequency.hpp>
#include <huffman_tree.hpp>
#include <genbitstream.hpp>
#include <memory>
#include <fstream>
#include <iostream>
#include <custream.hpp>
using namespace hmcmpsr;
using namespace std;

int main([[maybe_unused]]int argc,[[maybe_unused]]char **argv)
{
	char_frequency_t fre;
	auto bitout=genbitsaver::construct(2);
	auto bitin=genbitloader::construct(2);
	ifstream fin("test.txt");
	ofstream fout("test.hmc");
	auto cuin=icustream::construct(8,fin);
	fre.staticize(*cuin);
	cuin->clear();
	auto filesize=fin.tellg();
	cout<<filesize<<endl;
	fin.seekg(0);
	huffman_tree_base &tree=*(new huffman_tree);
	tree.build_tree(fre,2);
	tree.encode(*bitout,*cuin,filesize);
	bitout->save(fout);
	fin.close();fout.close();

	fin.open("test.hmc");
	fout.open("test2.txt");
	auto cuout=ocustream::construct(8,fout);
	bitin->load(fin);
	tree.decode(*cuout,*bitin);
	delete &tree;
	return 0;
}
