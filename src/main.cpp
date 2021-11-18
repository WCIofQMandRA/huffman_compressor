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
	single_cmpsr compressor;
	compressor.open(std::filesystem::u8path("test.txt"),2,8);
	compressor.compress(std::filesystem::u8path("test.hmz"));
	compressor.close();

	single_dcmpsr decompressor;
	decompressor.open(std::filesystem::u8path("test.hmz"));
	decompressor.decompress(std::filesystem::u8path("test2.txt"));
	decompressor.close();
	return 0;
}
