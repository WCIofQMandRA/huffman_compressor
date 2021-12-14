#CMakeLists.txt
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
# This software is free software, you can use and 
# distrubute it under the term of the BSD 3-Clause
# License, see the file “LICENSE” for detail.

cmake_minimum_required(VERSION 3.4)
include(TestBigEndian)
project(huffman_compressor VERSION 1.0.1)
set(CMAKE_CXX_STANDARD 17)

option (NO_WARNINGS "关闭所有警告" OFF)

if(NO_WARNINGS)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		add_compile_options(-w)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		add_compile_options(-w -Zc:__cplusplus -utf-8)
	endif()
else()
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		add_compile_options(-Wall -Wextra -Wpedantic -Wformat=2 -Wconversion)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		add_compile_options(-W4 -Zc:__cplusplus -utf-8)
	endif()
endif()

set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)

add_subdirectory(lib)
add_subdirectory(ext/nowide)
add_subdirectory(src)
