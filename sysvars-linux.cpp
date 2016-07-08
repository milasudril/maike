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
#include "filein.hpp"
#include "readbuffer.hpp"
#include <sys/utsname.h>
#include <cstring>
#include <cstdint>
#include <cstdlib>

#ifdef __unix__
#include <unistd.h>
#endif

using namespace Maike;

static void trim(std::string & str)
	{
	while (str.length () > 0 && (str.back()>='\0' && str.back()<=' '))
		{str.erase (str.length ()-1, 1);}
	}

static void cpufeaturesLoad(std::map<Stringkey,Variant>& variables)
	{
	FileIn source("/proc/cpuinfo");
	ReadBuffer rb(source);
	enum class State:unsigned int{KEY,VALUE};
	auto state=State::KEY;
	std::string buffer;
	while(!rb.eof())
		{
		char ch_in=rb.byteRead();
		switch(state)
			{
			case State::KEY:
				switch(ch_in)
					{
					case ':':
						state=State::VALUE;
						trim(buffer);
						printf("[%s] ",buffer.c_str());
						buffer.clear();
						break;
					case '\n':
						return; //Stop after reading one node
					default:
						if(ch_in>='\0' && ch_in<=' ')
							{
							if(buffer.length()>0)
								{buffer+=ch_in;}
							}
						else
							{buffer+=ch_in;}
					}
				break;

			case State::VALUE:
				switch(ch_in)
					{
					case '\n':
						trim(buffer);
						printf("[%s]\n",buffer.c_str());
						buffer.clear();
						state=State::KEY;
						break;
					default:
						if(ch_in>='\0' && ch_in<=' ')
							{
							if(buffer.length()>0)
								{buffer+=ch_in;}
							}
						else
							{buffer+=ch_in;}
					}
				break;
			}
		}
	}

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

	cpufeaturesLoad(variables);
	}
