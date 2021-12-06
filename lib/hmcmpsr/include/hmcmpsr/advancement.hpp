//advancement.hpp: 输出耗时操作的进度信息
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#pragma once
#include <iosfwd>

#ifdef HMCMPSR_DLLEXPORT
#   define HMCMPSR_API __declspec(__dllexport__)
#else
#   define HMCMPSR_API
#endif

namespace hmcmpsr
{
void HMCMPSR_API print_advancement(std::ostream&);
}

#undef HMCMPSR_API
