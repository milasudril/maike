//@	[["posix",
//@		{"targets":[{"name":"strerror.o","type":"object"}]}
//@	]]

#include "strerror.hpp"

#ifdef _GNU_SOURCE

extern "C"
    {
    extern int __xpg_strerror_r(int errcode,char* buffer,size_t length);
    #define strerror_r __xpg_strerror_r
    }

#else

#include <cstring>

#endif

using namespace Maike;

strerror::strerror(ErrCode errcode) noexcept
	{
	strerror_r(errcode,buffer,N);
	buffer[N-1]=0;
	}
