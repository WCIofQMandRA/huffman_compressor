//advancement.cpp: 输出耗时操作的进度信息
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#include <iostream>
#include <mutex>

namespace hmcmpsr
{
std::mutex adv_ostream_mutex;
std::ostream *adv_ostream;

void print_advancement(std::ostream &os)
{
    if(adv_ostream==nullptr)
    {
        std::unique_lock _(adv_ostream_mutex);
        adv_ostream=&os;
    }
}
}
