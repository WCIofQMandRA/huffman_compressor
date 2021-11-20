//char_frequency.cpp: 每种字符出现的频率
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <hmcmpsr/char_frequency.hpp>
#include <hmcmpsr/custream.hpp>
#include <iostream>
#include "print_advancement_stream.hpp"
#include <sstream>
#include <iomanip>
#include <chrono>

namespace hmcmpsr
{
void char_frequency_t::staticize(icustream &is,uint64_t filelength)
{
    namespace ck=std::chrono;
    using namespace std::chrono_literals;
    auto start_time=ck::system_clock::now();
    m_culen=is.get_culen();
    uint64_t current_char=0;
    while(true)
    {
        if(adv_ostream!=nullptr)
        {
            if(adv_ostream_mutex.try_lock())
            {
                std::ostringstream sout;
                sout<<std::setprecision(2)<<std::fixed;
                auto currentlength=is.tellg();
                auto time_duration=ck::system_clock::now()-start_time;
                auto second_cost=ck::duration_cast<ck::seconds>(time_duration).count();
                decltype(second_cost) second_left;
                sout<<"正在统计字符频率  "<<currentlength<<"/"<<filelength;
                if(filelength)
                {
                    double tmp=(double)currentlength/(double)filelength;
                    sout<<"("<<100.0*tmp<<"%)";
                    second_left=second_cost/tmp-second_cost;
                }
                
                sout<<"\n已耗时 "<<std::setfill('0')<<std::setw(2)<<second_cost/3600
                    <<":"<<std::setw(2)<<second_cost/60%60
                    <<":"<<std::setw(2)<<second_cost%60;
                if(filelength)
                {
                    sout<<"    剩余 "<<std::setfill('0')<<std::setw(2)<<second_left/3600
                    <<":"<<std::setw(2)<<second_left/60%60
                    <<":"<<std::setw(2)<<second_left%60;
                }
                *adv_ostream<<sout.str()<<std::endl;
                adv_ostream=nullptr;
                adv_ostream_mutex.unlock();
            }
        }
        is>>current_char;
        if(!is)break;
        ++(*this)[current_char];
    }
}
}
