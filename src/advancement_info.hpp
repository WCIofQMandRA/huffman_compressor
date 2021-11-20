//advancement_info.hpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.

#pragma once

//改变SIGINT的响应函数，在收到SIGINT时输出压缩/解压进度信息
void catch_keyboardint();
