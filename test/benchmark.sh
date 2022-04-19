#!/bin/bash

export LD_LIBRARY_PATH=/usr/riscv64-linux-gnu/lib:$LD_LIBRARY_PATH
export TEST_LIST="01 02 03 04 05 06"

echo "0. build"
cmake .. -B build-x86-64 -DCMAKE_BUILD_TYPE=Release -DNO_WARNINGS=ON
cd build-x86-64
make -j$(nproc)
cd ..

cmake .. -B build-riscv64 -DCMAKE_BUILD_TYPE=Release -DNO_WARNINGS=ON -DCMAKE_C_COMPILER=riscv64-linux-gnu-gcc -DCMAKE_CXX_COMPILER=riscv64-linux-gnu-g++
cd build-riscv64
make -j$(nproc)
cd ..

rm -f data/*.new data/*.hmz

echo "1. x86-64"
echo "压缩"
for n in $TEST_LIST
do
	echo "huffman_compressor -c data/test$n"
	time build-x86-64/output/bin/huffman_compressor -c data/test$n
done
echo "解压"
for n in $TEST_LIST
do
	echo "huffman_compressor -x data/test$n.hmz -o data/test$n.new"
	time build-x86-64/output/bin/huffman_compressor -x data/test$n.hmz -o data/test$n.new
done

rm -f data/*.new data/*.hmz

echo "----------------------------------------------------"
echo "2. riscv64"
echo "压缩"
for n in $TEST_LIST
do
	echo "huffman_compressor -c data/test$n"
	time build-riscv64/output/bin/huffman_compressor -c data/test$n
done
echo "解压"
for n in $TEST_LIST
do
	echo "huffman_compressor -x data/test$n.hmz -o data/test$n.new"
	time build-riscv64/output/bin/huffman_compressor -x data/test$n.hmz -o data/test$n.new
done
