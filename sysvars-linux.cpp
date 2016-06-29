//@	[
//@		[
//@		"linux"
//@			,{"targets":[{"name":"sysvars.o","type":"object"}]}
//@		]
//@	]

#include "sysvars.hpp"
#include "versionnumber.hpp"
#include <sys/utsname.h>
#include <cstring>
#include <cstdint>
#include <cstdlib>

#ifdef __unix__
#include <unistd.h>
#endif

void Maike::sysvarsLoad(std::map<Stringkey, int64_t>& variables)
	{
	utsname sysname;
	uname(&sysname);
	auto ver=version(sysname.release);
	variables[Stringkey("linux")]=ver;
#if __ANDROID__
	variables[Stringkey("android")]=__ANDROID_API__;
#elif __gnu_linux__
	variables[Stringkey("gnu")]=1;
#endif

#ifdef __unix__
	variables[Stringkey("posix")]=_POSIX_VERSION;
#endif
	}
