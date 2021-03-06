#src/CMakeLists.txt
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
# This software is free software, you can use and 
# distrubute it under the term of the BSD 3-Clause
# License, see the file “LICENSE” for detail.

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} SRC_HUFFMAN)

add_executable(huffman_compressor ${SRC_HUFFMAN})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
	set_target_properties(huffman_compressor PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(huffman_compressor PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
  	set_target_properties(huffman_compressor PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(huffman_compressor PROPERTIES INSTALL_RPATH "@loader_path/../lib")
endif()

target_link_libraries(huffman_compressor hmcmpsr nowide)

set_property(TARGET huffman_compressor PROPERTY RUNTIME_OUTPUT_DIRECTORY $<1:${OUTPUT_DIR}/bin>)
