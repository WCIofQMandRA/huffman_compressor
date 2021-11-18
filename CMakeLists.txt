#CMakeLists.txt
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
cmake_minimum_required(VERSION 3.4)
include(TestBigEndian)
project(huffman_compressor VERSION 1.0.0.0)
set(CMAKE_CXX_STANDARD 17)

option(USE_MINI_GMP "强制使用mini-gmp编译" OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wformat=2 -Wconversion)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(-W4 -Zc:__cplusplus -utf-8)
endif()

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)

add_subdirectory(lib)
add_subdirectory(nowide)
add_subdirectory(src)
