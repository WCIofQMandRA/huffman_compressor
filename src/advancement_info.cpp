//advancement_info.hpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the Huffman压缩器.
// This software is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#include <signal.h>
#include <unistd.h>
#include <chrono>
#include <nowide/iostream.hpp>
#include "exit.hpp"
#include <hmcmpsr/advancement.hpp>

static std::chrono::system_clock::time_point last_int_time;
static bool is_interactive_stdin;

void keyboardint_handler(int)
{
    if(is_interactive_stdin)
    {
        using namespace std::chrono_literals;
        auto now_time=std::chrono::system_clock::now();
        if(now_time-last_int_time<=500ms)
        {
            nowide::cerr<<"退出程序？(y/n)   "<<std::flush;
            while(true)
            {
                int ch=getchar();
                if(ch=='y'||ch=='Y')throw exit_type{3};
                else if(ch=='n'||ch=='N')return;
            }
        }
        last_int_time=now_time;
        nowide::cerr<<"若要退出，请在0.5s内按两次Ctrl-C。"<<std::endl;
    }
    hmcmpsr::print_advancement(nowide::clog,is_interactive_stdin);
}

//改变SIGINT的响应函数，在收到SIGINT时输出压缩/解压进度信息
void catch_keyboardint()
{
    is_interactive_stdin=isatty(fileno(stdin));
    signal(SIGINT,keyboardint_handler);
}
