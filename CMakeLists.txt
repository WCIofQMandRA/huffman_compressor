#CMakeLists.txt
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
cmake_minimum_required(VERSION 3.4)
project(huffman_compressor)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic -Wformat=2 -Wconversion)

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)

add_subdirectory(src)
