#!/bin/bash

#test-basic.sh: 测试基本的压缩和解压功能
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
# This software is free software, you can use and 
# distrubute it under the term of the BSD 3-Clause
# License, see the file “LICENSE” for detail.

echo "===================================================="
echo "test-basic"
echo "系统信息:"
uname -svrp
echo -n "测试开始时间:"
date --utc +"%F %T UTC"
echo "===================================================="
echo ""
echo "0. build"
echo "cmake .. -B build-nostl -DCMAKE_BUILD_TYPE=Release -DNO_WARNINGS=ON -DNO_STL_CONTAINER=ON"
cmake .. -B build-nostl -DCMAKE_BUILD_TYPE=Release -DNO_WARNINGS=ON -DNO_STL_CONTAINER=ON
echo "cd build-nostl && make -j4"
cd build-nostl
make -j4
cd ..

echo "cmake .. -B build-withstl -DCMAKE_BUILD_TYPE=Release -DNO_WARNINGS=ON"
cmake .. -B build-withstl -DCMAKE_BUILD_TYPE=Release -DNO_WARNINGS=ON
echo "cd build-withstl && make -j4"
cd build-withstl
make -j4
cd ..

rm -f data/*.new data/*.hmz

echo "----------------------------------------------------"
echo "1. NO_STL_CONTAINER=ON"
echo "压缩"
for n in 01 02 03 04 05 06 07
do
	echo "huffman_compressor -c data/test$n"
	time build-nostl/output/bin/huffman_compressor -c data/test$n
done
echo "解压"
for n in 01 02 03 04 05 06 07
do
	echo "huffman_compressor -x data/test$n.hmz -o data/test$n.new"
	time build-nostl/output/bin/huffman_compressor -x data/test$n.hmz -o data/test$n.new
done
echo "校验"
ls -l data/test*.hmz
for n in 01 02 03 04 05 06 07
do
	sha1sum data/test$n data/test$n.new
done

rm -f data/*.new data/*.hmz

echo "----------------------------------------------------"
echo "2. NO_STL_CONTAINER=OFF"
echo "压缩"
for n in 01 02 03 04 05 06 07
do
	echo "huffman_compressor -c data/test$n"
	time build-withstl/output/bin/huffman_compressor -c data/test$n
done
echo "解压"
for n in 01 02 03 04 05 06 07
do
	echo "huffman_compressor -x data/test$n.hmz -o data/test$n.new"
	time build-withstl/output/bin/huffman_compressor -x data/test$n.hmz -o data/test$n.new
done
echo "校验"
ls -l data/test*.hmz
for n in 01 02 03 04 05 06 07
do
	sha1sum data/test$n data/test$n.new
done

echo "----------------------------------------------------"
echo "3. 压缩包信息和Huffman树"
for n in 01 02 03 04 05 06 07
do
	echo "huffman_compressor -iTx data/test$n.hmz"
	build-withstl/output/bin/huffman_compressor -iTx data/test$n.hmz > tree/test$n-basic.txt
done

rm -f data/*.new data/*.hmz

echo -n "测试结束时间:"
date --utc +"%F %T UTC"
