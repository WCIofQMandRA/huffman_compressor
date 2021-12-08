//print_advancement_stream.hpp:
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#include <iosfwd>
#include <mutex>
#include <utility>

namespace hmcmpsr
{
extern std::mutex adv_ostream_mutex;
extern std::pair<std::ostream*,bool> adv_ostream;
}
