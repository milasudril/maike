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
	variables.insert({Stringkey("linux"), ver});
#if __ANDROID__
	variables.insert({Stringkey("android"),__ANDROID_API__});
#elif __gnu_linux__
	variables.insert({Stringkey("gnu"),1});
#endif

#ifdef __unix__
	variables.insert({Stringkey("posix"),_POSIX_VERSION});
#endif

	variables.insert({Stringkey("nullfile"),"/dev/null"});
	}
