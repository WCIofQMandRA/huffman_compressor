#!/bin/bash

#branch.sh: 测试不同分叉数的Huffman树
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
# This software is free software, you can use and 
# distrubute it under the term of the BSD 3-Clause
# License, see the file “LICENSE” for detail.

echo "===================================================="
echo "branch"
echo "系统信息:"
uname -svrp
echo "GMP版本:"
random_filename=`shuf -i 10000000-99999999 -n 1`
(echo "I2luY2x1ZGUgPHN0ZGlvLmg+CiNpbmNsdWRlIDxnbXAuaD4KCmludCBtYWluKCkKewoJcHJpbnRmKCJHTVAgdiVkLiVkLiVkLCBjb21waWxlZCBieSAiIF9fR01QX0NDICIgd2l0aCBmbGFncyBcIiIgX19HTVBfQ0ZMQUdTICJcIlxuIiwKCQlfX0dOVV9NUF9WRVJTSU9OLF9fR05VX01QX1ZFUlNJT05fTUlOT1IsX19HTlVfTVBfVkVSU0lPTl9QQVRDSExFVkVMKTsKCXJldHVybiAwOwp9Cg==" | base64 -d | gcc -x c - -o gmp-version$random_filename) && ./gmp-version$random_filename && rm gmp-version$random_filename
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
	for n in "002" "003" "004" "005" "006" "007" "008" "009" "010" "011" "012" "013" "014" "015" "016" "017" "018" "019" "020" "021" "127" "128" "255" "256"
	do
		echo "time huffman_compressor -c data/test$m -B $n -o data/test$m.hmz-$n"
		time build-withstl/output/bin/huffman_compressor -c data/test$m --branch $n -o data/test$m.hmz-$n
	done
	
	echo ""
	echo ""
	echo "解压"
	for n in "002" "003" "004" "005" "006" "007" "008" "009" "010" "011" "012" "013" "014" "015" "016" "017" "018" "019" "020" "021" "127" "128" "255" "256"
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
	for n in "002" "003" "004" "005" "006" "007" "008" "009" "010" "011" "012" "013" "014" "015" "016" "017" "018" "019" "020" "021" "127" "128" "255" "256"
	do
		echo "huffman_compressor -iTx data/test$m.hmz-$n"
		build-withstl/output/bin/huffman_compressor -iTx data/test$m.hmz-$n > tree/test$m-B$n.txt
	done
	
	rm -f data/test$m-* data/test$m.hmz-*
done

echo -n "测试结束时间:"
date --utc +"%F %T UTC"
