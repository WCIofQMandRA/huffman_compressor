#CMakeList.txt
#Copyright (C) 2021-2022 张子辰
#This file is part of the hmcmpsr library.
# This library is free software, you can use and 
# distrubute it under the term of the BSD 3-Clause
# License, see the file “LICENSE” for detail.

cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 17)
project(hmcmpsr VERSION 1.0.0.0)

option(USE_MINI_GMP "强制使用mini-gmp编译" OFF)
option(NO_STL_CONTAINER "不使用STL容器，目前使用与不使用此选项编译的库的源代码和二进制都不兼容" OFF)

TEST_BIG_ENDIAN(CHECK_BIG_ENDIAN)
if(USE_MINI_GMP)
	set(CHECK_HAS_GMP FALSE)
else()
	try_compile(CHECK_HAS_GMP ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_LIST_DIR}/check/detect_gmp.c)
endif()

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/sources HMCMPSR_SRC)
add_library(hmcmpsr SHARED ${HMCMPSR_SRC})
target_include_directories(hmcmpsr PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_definitions(hmcpsr PRIVATE HMCMPSR_DLLEXPORT)
endif()

if(CHECK_BIG_ENDIAN)
	target_compile_definitions(hmcmpsr PRIVATE HMCMPSR_BIG_ENDIAN)
endif()

if(CHECK_HAS_GMP)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		target_link_libraries(hmcmpsr PRIVATE -defaultlib:gmp)
	else()
		target_link_libraries(hmcmpsr PRIVATE -lgmp)
	endif()
else()
	target_include_directories(hmcmpsr PRIVATE ${CMAKE_CURRENT_LIST_DIR}/ext/mini-gmp)
	target_sources(hmcmpsr PRIVATE ${CMAKE_CURRENT_LIST_DIR}/ext/mini-gmp/mini-gmp.c)
	target_compile_definitions(hmcmpsr PRIVATE HMCPSR_NOGMP)
endif()

if(NO_STL_CONTAINER)
	target_include_directories(hmcmpsr PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ext/containers/include)
	target_compile_definitions(hmcmpsr PUBLIC HMCPSR_NOSTL)
endif()
