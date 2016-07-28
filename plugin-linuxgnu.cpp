//@	[
//@		[
//@		"and(linux,not(less_than(gnu,version('2.3.4'))))"
//@			,{
//@			 "targets":
//@				[{
//@				 "name":"plugin.o","type":"object"
//@				 ,"dependencies":[{"ref":"dl","rel":"external"}]
//@				}]
//@			}
//@		]
//@	]

#include "plugin.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "strerror.hpp"
#include "pathutils.hpp"
#include "filein.hpp"
#include "readbuffer.hpp"

#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#include <cstring>
#include <vector>

using namespace Maike;

static std::string exename()
	{
//TODO: This function belongs to pathutils
	ssize_t N=1;
	ssize_t n=0;
	std::string ret;
	while(N-1==n)
		{
		N<<=1;
		ret.resize(N);
		n=readlink("/proc/self/exe",&ret[0],N-1);
		if(n==-1)
			{
			exceptionRaise(ErrorMessage("It was not possible to resolve the path to Maike. #0;"
				,{static_cast<const char*>(Maike::strerror(errno))}));
			}
		}
	ret.resize(n);
	return std::move(ret);
	}

static std::string getcwd()
	{
//TODO: This function belongs to pathutils
	ssize_t N=1;
	std::string ret;
	char* result=NULL;
	while(result==NULL)
		{
		ret.resize(N);
		result=getcwd(&ret[0],N+1);
		N<<=1;
		}
	ret.resize(strlen(ret.c_str()));
	return std::move(ret);
	}

static std::string homedir()
	{
//TODO: This function belongs to pathutils
	FileIn src("/etc/passwd");
	ReadBuffer rb(src);
	std::string field;
	auto uid=getuid();
	decltype(uid) uid_from_file;
	std::string ret;
	size_t field_count=0;
	while(!rb.eof())
		{
		auto ch_in=rb.byteRead();
		switch(ch_in)
			{
			case ':':
				switch(field_count)
					{
					case 2:
						uid_from_file=atoi(field.c_str());
						break;
					case 5:
						ret=field;
						break;
					default:
						break;
					}
				++field_count;
				field.clear();
				break;
			case '\n':
				if(uid_from_file==uid)
					{return std::move(ret);}
				field_count=0;
				break;
			default:
				field+=ch_in;
			}
		}
	return std::string("");
	}

static void* open(const std::vector<std::string>& names_to_try,std::string& name)
	{
	auto ptr=names_to_try.data();
	auto ptr_end=ptr + names_to_try.size();
	void* ret=NULL;
	while(ptr!=ptr_end)
		{
		ret=dlopen(ptr->c_str(),RTLD_NOW|RTLD_DEEPBIND);
		if(ret!=NULL)
			{
			name=*ptr;
			return ret;
			}
		++ptr;
		}
	return ret;
	}

Plugin::Plugin(const char* name):m_name(name)
	{
	std::vector<std::string> names_to_try;
	if(strchr(name,'/')==nullptr)
		{
		names_to_try.push_back(dircat(dirname(exename()),name)+".so");
		names_to_try.push_back(dircat(std::string("/usr/local/lib/maike"),name)+".so");
		names_to_try.push_back(dircat(std::string("/usr/lib/maike"),name)+".so");
		names_to_try.push_back(dircat(dircat(homedir(),"lib/maike"),name)+".so");
		names_to_try.push_back(dircat(getcwd(),name)+".so");
		}
	else
		{names_to_try.push_back(m_name+".so");}

	m_handle=open(names_to_try,m_name_full);
	if(m_handle==NULL)
		{
		exceptionRaise(ErrorMessage("It was not possible to load #0;. #1;"
			,{m_name.c_str(),dlerror()}));
		}
	}

Plugin::~Plugin()
	{
	if(m_handle!=NULL)
		{dlclose(m_handle);}
	}

Plugin::Plugin(Plugin&& obj) noexcept:
	m_name_full(obj.m_name_full),m_name(obj.m_name),m_handle(obj.m_handle)
	{obj.m_handle=NULL;}

Plugin& Plugin::operator=(Plugin&& obj) noexcept
	{
	std::swap(m_handle,obj.m_handle);
	std::swap(m_name,obj.m_name);
	std::swap(m_name_full,obj.m_name_full);
	return *this;
	}

Plugin::Function Plugin::functionGet(const char* name) const
	{
	auto ret=dlsym(m_handle,name);
	if(ret==NULL)
		{
		exceptionRaise(ErrorMessage("#0;: The function #1; does not exist."
			,{m_name.c_str(),name}));
		}
	return reinterpret_cast<Function>(ret);
	}

bool Plugin::dead() const noexcept
	{return m_handle==NULL;}
