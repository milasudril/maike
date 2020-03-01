//@	[
//@		[
//@		"linux"
//@			,{"targets":[{"name":"pathutils.o","type":"object"}]}
//@		]
//@	]

#include "pathutils.hpp"
#include "exceptionhandler.hpp"
#include "variant.hpp"
#include "errormessage.hpp"
#include "strerror.hpp"
#include "filein.hpp"
#include "readbuffer.hpp"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <cstring>
#include <unistd.h>

using namespace Maike;

PRIVATE std::string Maike::directoryNormalize(const char* str)
{
	std::string ret;
	std::string elem_current;
	bool up_more = 0;
	const char* base = str;
	while(true)
	{
		auto ch_in = *str;
		switch(ch_in)
		{
			case '\0':
			case '/':
				if(elem_current == "..")
				{
					if(ret.size() == 0 || up_more)
					{
						if(*base == '/') { exceptionRaise(ErrorMessage("#0; is a malformed path.", {base})); }
						ret += elem_current;
						ret += '/';
						up_more = 1;
					}
					else
					{
						ret.erase(ret.size() - 1, 1);
						auto pos = ret.find_last_of('/');
						if(pos == ret.npos) { ret.clear(); }
						else
						{
							ret.erase(ret.find_last_of('/')) += '/';
						}
					}
					if(ch_in == '\0') { return std::move(ret); }
				}
				else if(elem_current == ".")
				{
					if(ch_in == '\0')
					{
						if(ret.size() == 0) { return std::string("."); }
						auto pos = ret.find_last_of('/');
						if(pos == std::string::npos) { return std::move(ret); }
						return ret.substr(0, pos);
					}
				}
				else
				{
					ret.append(elem_current);
					if(ch_in == '\0') { return ret.size() ? std::move(ret) : std::string("."); }
					ret += '/';
					up_more = 0;
				}

				elem_current.clear();
				break;

			default: elem_current += ch_in;
		}
		++str;
	}
	return std::string(".");
}

PRIVATE std::string Maike::rootStrip(const char* path, const char* root)
{
	auto ptr = path;
	auto ch_root_prev = '/';
	std::string ret;
	while(true)
	{
		auto ch_in = *ptr;
		auto ch_root = *root;
		if(ch_in == '\0')
		{
			ret = std::string(ptr);
			break;
		}
		if(ch_root == '\0' && ch_root_prev != '/')
		{
			ret = std::string(ptr + 1);
			break;
		}
		if(ch_in != ch_root)
		{
			ret = std::string(ptr);
			break;
		}
		ch_root_prev = ch_root;
		++ptr;
		++root;
	}
	return ret.size() ? std::move(ret) : std::string(".");
}

std::string Maike::exename()
{
	ssize_t N = 1;
	ssize_t n = 0;
	std::string ret;
	while(N - 1 == n)
	{
		N <<= 1;
		ret.resize(N);
		n = readlink("/proc/self/exe", &ret[0], N - 1);
		if(n == -1)
		{
			exceptionRaise(ErrorMessage("It was not possible to resolve the path to Maike. #0;",
			                            {static_cast<const char*>(Maike::strerror(errno))}));
		}
	}
	ret.resize(n);
	return std::move(ret);
}

std::string Maike::getcwd()
{
	ssize_t N = 1;
	std::string ret;
	char* result = NULL;
	while(result == NULL)
	{
		ret.resize(N);
		result = ::getcwd(&ret[0], N + 1);
		N <<= 1;
	}
	ret.resize(strlen(ret.c_str()));
	return std::move(ret);
}

std::string Maike::homedir()
{
	FileIn src("/etc/passwd");
	ReadBuffer rb(src);
	std::string field;
	auto uid = getuid();
	decltype(uid) uid_from_file = 0;
	std::string ret;
	size_t field_count = 0;
	while(!rb.eof())
	{
		auto ch_in = rb.byteRead();
		switch(ch_in)
		{
			case ':':
				switch(field_count)
				{
					case 2: uid_from_file = atoi(field.c_str()); break;
					case 5: ret = field; break;
					default: break;
				}
				++field_count;
				field.clear();
				break;
			case '\n':
				if(uid_from_file == uid) { return std::move(ret); }
				field_count = 0;
				break;
			default: field += ch_in;
		}
	}
	return std::string("");
}

std::string Maike::configdir()
{
	auto exe = exename();
	auto ret = dircat(dirname(exename()), "../share");
	return dircat(ret, basename(exe.c_str()));
}

std::string Maike::configdirUser()
{
	auto home = homedir();
	auto ret = dircat(home, ".config");
	auto exe = exename();
	return dircat(ret, basename(exe.c_str()));
}
