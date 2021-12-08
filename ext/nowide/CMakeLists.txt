cmake_minimum_required(VERSION 3.4)
set(CMAKE_CXX_STANDARD 17)
project(nowide VERSION 1.73.0)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/source NOWIDE_SRC)

add_library(nowide SHARED ${NOWIDE_SRC})
target_include_directories(nowide PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

#大部分情况下，即使不加dllimport也会尝试在dll中寻找符号，但全局变量是一个例外。
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_definitions(nowide PUBLIC NOWIDE_DYN_LINK)
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
	set_target_properties(nowide PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(nowide PROPERTIES INSTALL_RPATH "\$ORIGIN/")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
  	set_target_properties(nowide PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(nowide PROPERTIES INSTALL_RPATH "@loader_path/")
endif()

set_property(TARGET nowide PROPERTY LIBRARY_OUTPUT_DIRECTORY $<1:${OUTPUT_DIR}/lib>)
set_property(TARGET nowide PROPERTY RUNTIME_OUTPUT_DIRECTORY $<1:${OUTPUT_DIR}/bin>)
