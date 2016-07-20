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

#include <dlfcn.h>
#include <limits.h>
#include <unistd.h>
#include <cstring>

using namespace Maike;

static std::string exename()
	{
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
	return std::move(ret);
	}

Plugin::Plugin(const char* name):m_name(name)
	{
	if(strchr(name,'/')==nullptr)
		{m_name_full=dircat(dirname(exename()),name);}
	else
		{m_name_full=name;}
	m_name_full+=".so";
	m_handle=dlopen(m_name_full.c_str(),RTLD_NOW|RTLD_DEEPBIND);
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
	m_name(obj.m_name),m_handle(obj.m_handle)
	{obj.m_handle=NULL;}

Plugin& Plugin::operator=(Plugin&& obj) noexcept
	{
	std::swap(m_handle,obj.m_handle);
	std::swap(m_name,obj.m_name);
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
