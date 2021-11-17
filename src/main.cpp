//main.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
#include <memory>
#include <fstream>
#include <iostream>
#include <hmcompressor.hpp>
using namespace hmcmpsr;
using namespace std;
const int branch=2,culen=8;

int main([[maybe_unused]]int argc,[[maybe_unused]]char **argv)
{
	ifstream fin("test.txt");
	ofstream fout("test.hmz");
	single_cmpsr compressor;
	compressor.n_branches=2;
	compressor.code_unit_length=8;
	compressor.data_block_size=1048576;
	compressor.compress(fout,fin);
	fin.close();
	fout.close();

	fin.open("test.hmz");
	fout.open("test2.txt");
	single_dcmpsr decompressor;
	decompressor.decompress(fout,fin);
	fin.close();
	fout.close();
	return 0;
}
