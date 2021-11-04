#src/CMakeLists.txt
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.

aux_source_directory(${CMAKE_CURRENT_LIST_DIR} SRC_HUFFMAN)

add_executable(huffman_compressor ${SRC_HUFFMAN})

set_target_properties(huffman_compressor PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(huffman_compressor PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")

set_property(TARGET huffman_compressor PROPERTY RUNTIME_OUTPUT_DIRECTORY $<1:${OUTPUT_DIR}/bin>)
