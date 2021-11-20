//advancement_info.hpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
#include <signal.h>
#include <chrono>
#include <nowide/iostream.hpp>
#include "exit.hpp"

static std::chrono::system_clock::time_point last_int_time;

void keyboardint_handler(int)
{
    using namespace std::chrono_literals;
    auto now_time=std::chrono::system_clock::now();
    if(now_time-last_int_time<=500ms)
        throw exit_type{3};
    last_int_time=now_time;
    nowide::cout<<"若要退出，请在0.5s内按两次Ctrl-C。"<<std::endl;
}

//改变SIGINT的响应函数，在收到SIGINT时输出压缩/解压进度信息
void catch_keyboardint()
{
    signal(SIGINT,keyboardint_handler);
}
