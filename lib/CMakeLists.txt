#lib/CMakeLists.txt
#Copyright 2021-2022 张子辰
#This file is part of the Huffman压缩器.
# This software is free software, you can use and 
# distrubute it under the term of the BSD 3-Clause
# License, see the file “LICENSE” for detail.

add_subdirectory(hmcmpsr VERSION 1.0.1)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
	set_target_properties(hmcmpsr PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(hmcmpsr PROPERTIES INSTALL_RPATH "\$ORIGIN/")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
  	set_target_properties(hmcmpsr PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(hmcmpsr PROPERTIES INSTALL_RPATH "@loader_path/")
endif()

set_property(TARGET hmcmpsr PROPERTY LIBRARY_OUTPUT_DIRECTORY $<1:${OUTPUT_DIR}/lib>)
set_property(TARGET hmcmpsr PROPERTY RUNTIME_OUTPUT_DIRECTORY $<1:${OUTPUT_DIR}/bin>)
