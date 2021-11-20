//print_advancement_stream.hpp:
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <iosfwd>
#include <mutex>

namespace hmcmpsr
{
extern std::mutex adv_ostream_mutex;
extern std::ostream *adv_ostream;
}
