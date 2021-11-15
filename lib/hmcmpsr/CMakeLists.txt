#CMakeList.txt
#Copyright (C) 2021-2022 张子辰
#This file is part of the hmcmpsr library.

cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 17)
project(hmcmpsr VERSION 1.0.0.0)

TEST_BIG_ENDIAN(CHECK_BIG_ENDIAN)
try_compile(CHECK_HAS_GMP ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_LIST_DIR}/check/detect_gmp.c)

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
	target_include_directories(hmcmpsr PRIVATE ${CMAKE_CURRENT_LIST_DIR}/mini-gmp)
	target_sources(hmcmpsr ${CMAKE_CURRENT_LIST_DIR}/mini-gmp/mini-gmp.c)
	target_compile_definitions(hmcmpsr PRIVATE HMCPSR_NOGMP)
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
	set_target_properties(hmcmpsr PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(hmcmpsr PROPERTIES INSTALL_RPATH "\$ORIGIN/")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
  	set_target_properties(hmcmpsr PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(hmcmpsr PROPERTIES INSTALL_RPATH "@loader_path/")
endif()

set_property(TARGET hmcmpsr PROPERTY LIBRARY_OUTPUT_DIRECTORY $<1:${OUTPUT_DIR}/lib>)
set_property(TARGET hmcmpsr PROPERTY RUNTIME_OUTPUT_DIRECTORY $<1:${OUTPUT_DIR}/bin>)
