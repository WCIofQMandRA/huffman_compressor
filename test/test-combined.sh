#!/bin/bash

#test-combined.sh: 测试任意组合--unit-length和--branch参数
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
# This software is free software, you can use and 
# distrubute it under the term of the BSD 3-Clause
# License, see the file “LICENSE” for detail.

echo "===================================================="
echo "test-combined"
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
for m in 01 03
do
	rm -f data/test$m-* data/test$m.hmz-*
	echo "----------------------------------------------------"
	echo "test$m"
	echo "压缩"
	for u in "01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16"
	do
		for b in "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16" "17"
		do
			echo "time huffman_compressor -c data/test$m -U $u -B $b -o data/test$m.hmz-U${u}B${b}"
			time build-withstl/output/bin/huffman_compressor -c data/test$m --unit-length $u --branch $b -o data/test$m.hmz-U${u}B${b}
		done
		
	done
	
	echo ""
	echo ""
	echo "解压"
	for u in "01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16"
	do
		for b in "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16" "17"
		do
			echo "time huffman_compressor -x data/test$m.hmz-U${u}B${b} -o data/test$m-U${u}B${b}"
			time build-withstl/output/bin/huffman_compressor -x data/test$m.hmz-U${u}B${b} -o data/test$m-U${u}B${b}
		done
	done
	
	echo ""
	echo ""
	echo "校验"
	ls -l data/test$m.hmz-*
	sha1sum data/test$m data/test$m-*
	
	echo ""
	echo ""
	echo "压缩包信息和Huffman树"
	for u in "01" "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16"
	do
		for b in "02" "03" "04" "05" "06" "07" "08" "09" "10" "11" "12" "13" "14" "15" "16" "17"
		do
			echo "huffman_compressor -iTx data/test$m.hmz-U${u}B${b}"
			build-withstl/output/bin/huffman_compressor -iTx data/test$m.hmz-U${u}B${b} > tree/test$m-U${u}B${b}.txt
		done
	done
	
	rm -f data/test$m-* data/test$m.hmz-*
done

echo -n "测试结束时间:"
date --utc +"%F %T UTC"
