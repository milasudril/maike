//@	[
//@		[
//@		"linux"
//@			,{"targets":[{"name":"sysvars.o","type":"object"}]}
//@		]
//@	]

#include "sysvars.hpp"
#include "versionnumber.hpp"
#include "stringkey.hpp"
#include "variant.hpp"
#include "mapreplace.hpp"
#include <sys/utsname.h>
#include <cstring>
#include <cstdint>
#include <cstdlib>

#ifdef __unix__
#include <unistd.h>
#endif

void Maike::sysvarsLoad(std::map<Stringkey, Variant>& variables)
	{
	utsname sysname;
	uname(&sysname);
	auto ver=version(sysname.release);
	replace(variables,{Stringkey("linux"), ver});

#if __ANDROID__
	replace(variables,{Stringkey("android"),__ANDROID_API__});
#elif __gnu_linux__
	replace(variables,{Stringkey("gnu"),1});
#endif

#ifdef __unix__
	replace(variables,{Stringkey("posix"),_POSIX_VERSION});
#endif

	replace(variables,{Stringkey("nullfile"),"/dev/null"});
	}
