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
#include "twins.hpp"

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

static void cpuinfoLoad(std::map<Stringkey,std::string>& info)
	{
	FileIn source("/proc/cpuinfo");
	ReadBuffer rb(source);
	enum class State:unsigned int{KEY,VALUE};
	auto state=State::KEY;
	std::string buffer;
	Stringkey key("");
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
						key=Stringkey(buffer.c_str());
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
						info[key]=buffer;
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



static void flagsLoad(const std::map<Stringkey,std::string>& cpuinfo
	,std::map<Stringkey,Variant>& variables
	,std::map<Stringkey,std::string>& varnames)
	{
	auto x=cpuinfo.end();
#if __x86_64 || __x86_64__ || __amd64 || __amd64__ || _M_X64 || _M_AMD64 \
	|| __i386__ || i386 || __i386 || __IA32__ || _M_IX86 || __X86__ || _X86_ \
	|| __386 || __INTEL__
	x=cpuinfo.find(Stringkey("flags"));
#elif __arm__ || __thumb__ || __TARGET_ARCH_ARM || __TARGET_ARCH_THUMB || _ARM \
	|| _M_ARM || _M_ARMT || __arm
	x=cpuinfo.find(Stringkey("features"));
#endif
	if(x!=cpuinfo.end())
		{
		auto ptr=x->second.c_str();
		std::string buffer("cpufeature_");
		while(true)
			{
			auto ch_in=*ptr;
			switch(ch_in)
				{
				case ' ':
					replace(variables,{Stringkey(buffer.c_str()),1});
					varnames[Stringkey(buffer.c_str())]=buffer;
					buffer=std::string("cpufeature_");
					break;
				case '\0':
					replace(variables,{Stringkey(buffer.c_str()),1});
					varnames[Stringkey(buffer.c_str())]=buffer;
					return;
				default:
					buffer+=ch_in;
				}
			++ptr;
			}
		}

	}

static void cpuinfoSet(const std::map<Stringkey,std::string>& cpuinfo
	,std::map<Stringkey,Variant>& variables
	,std::map<Stringkey,std::string>& varnames)
	{
	const std::pair<const char*,Stringkey> keymap[]=
		{
			 {"cpu_cache_size",Stringkey("cache size")}
			,{"cpu_cache_flushsize",Stringkey("clflush size")}
			,{"cpu_cache_alignment",Stringkey("cache_alignment")}
		};

	auto N_keys=sizeof(keymap)/sizeof(Twins<Stringkey>);
	while(N_keys!=0)
		{
		--N_keys;
		auto find=keymap[N_keys].second;
		auto i=cpuinfo.find(find);
		if(i!=cpuinfo.end())
			{
			replace(variables,{Stringkey(keymap[N_keys].first),atol(i->second.c_str())});
			varnames[Stringkey(keymap[N_keys].first)]=std::string(keymap[N_keys].first);
			}
		}
	}


static void cpuinfoSet(const std::map<Stringkey,std::string>& cpuinfo
	,std::map<Stringkey,Variant>& variables
	,std::map<Stringkey,std::string>& strings
	,std::map<Stringkey,std::string>& varnames)
	{
	const std::pair<const char*,Stringkey> keymap[]=
		{
			 {"cpu_vendor",Stringkey("vendor_id")}
		};

	auto N_keys=sizeof(keymap)/sizeof(Twins<Stringkey>);
	while(N_keys!=0)
		{
		--N_keys;
		auto find=keymap[N_keys].second;
		auto i=cpuinfo.find(find);
		if(i!=cpuinfo.end())
			{
			auto k=replace(strings,{Stringkey(keymap[N_keys].first),i->second});
			replace(variables,{Stringkey(keymap[N_keys].first),k->second.c_str()});
			varnames[Stringkey(keymap[N_keys].first)]=std::string(keymap[N_keys].first);
			}
		}
	}

void Maike::sysvarsLoad(std::map<Stringkey, Variant>& variables
	,std::map<Stringkey,std::string>& strings
	,std::map<Stringkey,std::string>& varnames)
	{
	utsname sysname;
	uname(&sysname);
	auto ver=version(sysname.release);
	replace(variables,{Stringkey("linux"), ver});
	varnames[Stringkey("linux")]=std::string("linux");
		{
		auto i=replace(strings,{Stringkey("linux_string"),sysname.release});
		replace(variables,{Stringkey("linux_string"),i->second.c_str()});
		varnames[Stringkey("linux_string")]=std::string("linux_string");
		}

#if __ANDROID__
	replace(variables,{Stringkey("android"),__ANDROID_API__});
	varnames[Stringkey("android")]=std::string("android");
#elif __gnu_linux__
	replace(variables,{Stringkey("gnu"),1});
	varnames[Stringkey("gnu")]=std::string("gnu");
#endif

#ifdef __unix__
	replace(variables,{Stringkey("posix"),_POSIX_VERSION});
	varnames[Stringkey("posix")]=std::string("posix");
#endif

	replace(variables,{Stringkey("nullfile"),"/dev/null"});
	varnames[Stringkey("nullfile")]=std::string("nullfile");

	std::map<Stringkey,std::string> cpuinfo;
	cpuinfoLoad(cpuinfo);
#if __x86_64 || __x86_64__ || __amd64 || __amd64__ || _M_X64 || _M_AMD64
	replace(variables,{Stringkey("wordsize"),64});
	varnames[Stringkey("wordsize")]=std::string("wordsize");
	replace(variables,{Stringkey("x86_64"),1});
	varnames[Stringkey("x86_64")]=std::string("x86_64");
	replace(variables,{Stringkey("architecture"),"x86_64"});
	varnames[Stringkey("architecture")]=std::string("architecture");
#elif __i386__ || i386 || __i386 || __IA32__ || _M_IX86 || __X86__ || _X86_ \
	|| __386 || __INTEL__
	replace(variables,{Stringkey("wordsize"),32});
	varnames[Stringkey("wordsize")]=std::string("wordsize");
	replace(variables,{Stringkey("i386"),1});
	varnames[Stringkey("i386")]=std::string("i386");
	replace(variables,{Stringkey("architecture"),"i386"});
	varnames[Stringkey("architecture")]=std::string("architecture");
#endif
	flagsLoad(cpuinfo,variables,varnames);
	cpuinfoSet(cpuinfo,variables,strings,varnames);
	cpuinfoSet(cpuinfo,variables,varnames);
	}
