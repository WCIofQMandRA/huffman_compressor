#!/bin/bash

#test-unit-length.sh: 测试不同长度的编码单元
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
# This software is free software, you can use and 
# distrubute it under the term of the BSD 3-Clause
# License, see the file “LICENSE” for detail.

echo "===================================================="
echo "test-unit-length"
echo "系统信息:"
uname -svrp
echo -n "测试开始时间:"
date --utc +"%F %T UTC"
echo "===================================================="
echo ""
echo "0. build"
echo "cmake .. -B build-withstl -DCMAKE_BUILD_TYPE=Release -DNO_WARNINGS=ON"
cmake .. -B build-withstl -DCMAKE_BUILD_TYPE=Release -DNO_WARNINGS=ON
echo "cd build-withstl && make -j4"
cd build-withstl
make -j4
cd ..

#为了节约时间只测试部分用例
for m in 01 02 03 04 06
do
	rm -f data/test$m-* data/test$m.hmz-*
	echo "----------------------------------------------------"
	echo "test$m"
	echo "压缩"
	for n in "01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16" "17" "18" "19" "20" "21" "22" "23" "24"
	do
		echo "time huffman_compressor -c data/test$m -U $n -o data/test$m.hmz-$n"
		time build-withstl/output/bin/huffman_compressor -c data/test$m --unit-length $n -o data/test$m.hmz-$n
	done
	
	echo ""
	echo ""
	echo "解压"
	for n in "01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16" "17" "18" "19" "20" "21" "22" "23" "24"
	do
		echo "time huffman_compressor -x data/test$m.hmz-$n -o data/test$m-$n"
		time build-withstl/output/bin/huffman_compressor -x data/test$m.hmz-$n -o data/test$m-$n
	done
	
	echo ""
	echo ""
	echo "校验"
	ls -l data/test$m.hmz-*
	sha1sum data/test$m data/test$m-*
	
	echo ""
	echo ""
	echo "压缩包信息和Huffman树"
	for n in "01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16" "17" "18" "19" "20" "21" "22" "23" "24"
	do
		echo "huffman_compressor -iTx data/test$m.hmz-$n"
		build-withstl/output/bin/huffman_compressor -iTx data/test$m.hmz-$n > tree/test$m-U$n.txt
	done
	
	rm -f data/test$m-* data/test$m.hmz-*
done




