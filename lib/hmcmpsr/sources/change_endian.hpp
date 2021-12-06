//change endian: 在大端序时交换端序
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#ifdef HMCMPSR_BIG_ENDIAN
template <typename IntType>
constexpr IntType ched(IntType x)
{
	char *px=reinterpret_cast<char*>(&x);
	static_assert(sizeof(x)<=8,"只支持64位及以下的整数");
	if constexpr(sizeof(x)==2)
	{
		char tmp=px[0];
		px[0]=px[1];
		px[1]=tmp;
	}
	else if constexpr(sizeof(x)==4)
	{
		char tmp0=px[0],tmp1=px[1];
		px[0]=px[3],px[1]=px[2];
		px[3]=tmp0,px[2]=tmp1;
	}
	else if constexpr(sizeof(x)==8)
	{
		char tmp0=px[0],tmp1=px[1],tmp2=px[2],tmp3=px[3];
		px[0]=px[7],px[1]=px[6],px[2]=px[5],px[3]=px[4];
		px[7]=tmp0,px[6]=tmp1,px[5]=tmp2,px[4]=tmp3;
	}
	return x;
}
#else
template<typename IntType>
constexpr IntType ched(IntType x){return x;}
#endif

