//@	 {"targets":[{"name":"targetcxxoptions.o","type":"object"}]}

#include "targetcxxoptions.hpp"
#include "../resourceobject.hpp"
#include "../exceptionhandler.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include <limits>

using namespace Maike;

static void stringArrayBuild(const ResourceObject& array,std::vector< std::string >& ret )
	{
	auto N=array.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{ret.push_back(std::string(static_cast<const char*>(array.objectGet(k))));}
	}

static void stringArrayGet(ResourceObject& ret, const std::vector< std::string >& array)
	{
	auto ptr=array.data();
	auto ptr_end=ptr + array.size();
	while(ptr!=ptr_end)
		{
		ret.objectAppend(ResourceObject(ptr->c_str()));
		++ptr;
		}
	}

TargetCxxOptions::TargetCxxOptions()
	{
	configClear();
	}

TargetCxxOptions::TargetCxxOptions(const ResourceObject& cxxoptions)
	{
	configClear();
	configAppend(cxxoptions);
	}

TargetCxxOptions& TargetCxxOptions::configAppend(const ResourceObject& cxxoptions)
	{
	if(cxxoptions.objectExists("includedir"))
		{stringArrayBuild(cxxoptions.objectGet("includedir"),m_includedir);}

	if(cxxoptions.objectExists("includedir_noscan"))
		{stringArrayBuild(cxxoptions.objectGet("includedir_noscan"),m_includedir_noscan);}

	if(cxxoptions.objectExists("libdir"))
		{stringArrayBuild(cxxoptions.objectGet("libdir"),m_libdir);}


	if(cxxoptions.objectExists("cxxversion_min"))
		{
		m_cxxversion_min=std::max( m_cxxversion_min
			,static_cast<unsigned long long int>(
				static_cast<long long int>( cxxoptions.objectGet("cxxversion_min") ) ) );
		}

	if(cxxoptions.objectExists("cxxversion_max"))
		{
		m_cxxversion_max=std::min( m_cxxversion_max
			,static_cast<unsigned long long int>(
				static_cast<long long int>( cxxoptions.objectGet("cxxversion_max") ) ) );
		}

	if(m_cxxversion_max < m_cxxversion_min)
		{
		exceptionRaise(ErrorMessage("cxxoptions: Inconsistent C++ version requirements",{}));
		}


	if(cxxoptions.objectExists("stdprefix"))
		{m_stdprefix=std::string( static_cast<const char*>(cxxoptions.objectGet("stdprefix")));}


	if(cxxoptions.objectExists("cflags_format"))
		{m_cflags_format=std::string(static_cast<const char*>(cxxoptions.objectGet("cflags_format")));}

	if(cxxoptions.objectExists("cflags_extra"))
		{stringArrayBuild(cxxoptions.objectGet("cflags_extra"),m_cflags_extra);}


	if(cxxoptions.objectExists("includedir_format"))
		{m_includedir_format=std::string(static_cast<const char*>(cxxoptions.objectGet("includedir_format")));}

	if(cxxoptions.objectExists("libdir_format"))
		{m_libdir_format=std::string(static_cast<const char*>(cxxoptions.objectGet("libdir_format")));}

	if(cxxoptions.objectExists("libext_format"))
		{m_libext_format=std::string(static_cast<const char*>(cxxoptions.objectGet("libext_format")));}

	if(cxxoptions.objectExists("libint_format"))
		{m_libint_format=std::string(static_cast<const char*>(cxxoptions.objectGet("libint_format")));}



	if(cxxoptions.objectExists("objcompile"))
		{m_objcompile=Command(cxxoptions.objectGet("objcompile"));}

	if(cxxoptions.objectExists("appcompile"))
		{m_appcompile=Command(cxxoptions.objectGet("appcompile"));}

	if(cxxoptions.objectExists("dllcompile"))
		{m_dllcompile=Command(cxxoptions.objectGet("dllcompile"));}

	if(cxxoptions.objectExists("libcompile"))
		{m_libcompile=Command(cxxoptions.objectGet("libcompile"));}

	if(cxxoptions.objectExists("versionquery"))
		{m_versionquery=Command(cxxoptions.objectGet("versionquery"));}

	if(cxxoptions.objectExists("pkgconfig"))
		{m_pkgconfig=Command(cxxoptions.objectGet("pkgconfig"));}

	return *this;
	}

template <typename T>
void append(std::vector<T>& a, const std::vector<T>& b)
	{
	a.reserve(a.size() + b.size());
    a.insert(a.end(), b.begin(), b.end());
	}


TargetCxxOptions& TargetCxxOptions::includedirNoscanAppend(const char* dir)
	{
	m_includedir_noscan.push_back(std::string(dir));
	return *this;
	}

TargetCxxOptions& TargetCxxOptions::configAppend(const TargetCxxOptions& cxxoptions)
	{
	append(m_includedir,cxxoptions.m_includedir);
	append(m_includedir_noscan,cxxoptions.m_includedir_noscan);
	append(m_libdir,cxxoptions.m_libdir);
	append(m_cflags_extra,cxxoptions.m_cflags_extra);

	m_cxxversion_min=std::max(m_cxxversion_min,cxxoptions.m_cxxversion_min);
	m_cxxversion_max=std::min(m_cxxversion_max,cxxoptions.m_cxxversion_max);
	if(m_cxxversion_max < m_cxxversion_min)
		{
		exceptionRaise(ErrorMessage("cxxoptions: Inconsistent C++ version requirements",{}));
		}
	if(cxxoptions.m_stdprefix.size()!=0)
		{m_stdprefix=cxxoptions.m_stdprefix;}

	if(cxxoptions.m_cflags_format.size()!=0)
		{m_cflags_format=cxxoptions.m_cflags_format;}


	if(cxxoptions.m_includedir_format.size()!=0)
		{m_includedir_format=cxxoptions.m_includedir_format;}

	if(cxxoptions.m_libdir_format.size()!=0)
		{m_libdir_format=cxxoptions.m_libdir_format;}

	if(cxxoptions.m_libext_format.size()!=0)
		{m_libext_format=cxxoptions.m_libext_format;}

	if(cxxoptions.m_libint_format.size()!=0)
		{m_libint_format=cxxoptions.m_libint_format;}


	if(cxxoptions.m_objcompile)
		{m_objcompile=cxxoptions.m_objcompile;}

	if(cxxoptions.m_appcompile)
		{m_appcompile=cxxoptions.m_appcompile;}

	if(cxxoptions.m_dllcompile)
		{m_dllcompile=cxxoptions.m_dllcompile;}

	if(cxxoptions.m_libcompile)
		{m_libcompile=cxxoptions.m_libcompile;}

	if(cxxoptions.m_versionquery)
		{m_versionquery=cxxoptions.m_versionquery;}

	if(cxxoptions.m_pkgconfig)
		{m_pkgconfig=cxxoptions.m_pkgconfig;}

	return *this;
	}

void TargetCxxOptions::configClear()
	{
	m_includedir.clear();
	m_includedir_noscan.clear();
	m_libdir.clear();
	m_objcompile.nameSet("").argumentsClear();
	m_cxxversion_min=0;
	m_cxxversion_max=std::numeric_limits<decltype(m_cxxversion_max)>::max();
	}

TargetCxxOptions& TargetCxxOptions::configAppendDefault()
	{
	m_cxxversion_min=__cplusplus;
	m_cxxversion_max=std::numeric_limits<decltype(m_cxxversion_max)>::max();;

	m_includedir_format=std::string("-I^");
	m_libdir_format=std::string("-L^");
	m_libext_format=std::string("-l^");
	m_libint_format=std::string("-l:^");
	m_stdprefix=std::string("-std=");
	m_cflags_format=std::string("-^");

	m_objcompile.argumentsClear();
	m_objcompile.nameSet("g++").argumentAppend("-c")
		.argumentAppend("-g").argumentAppend("-fpic")
		.argumentAppend("{cxxversion}").argumentAppend("-Wall")
		.argumentAppend("{cflags_extra}").argumentAppend("{includedir}")
		.argumentAppend("-o").argumentAppend("{target}")
		.argumentAppend("{source}");

	m_appcompile.argumentsClear();
	m_appcompile.nameSet("g++").argumentAppend("-g")
		.argumentAppend("-fpic").argumentAppend("{cxxversion}")
		.argumentAppend("-Wall").argumentAppend("{cflags_extra}")
		.argumentAppend("{includedir}")
		.argumentAppend("-o").argumentAppend("{target}")
		.argumentAppend("{source}").argumentAppend("{dependencies}");

	m_dllcompile.argumentsClear();
	m_dllcompile.nameSet("g++").argumentAppend("-g")
		.argumentAppend("-fpic").argumentAppend("{cxxversion}")
		.argumentAppend("-Wall").argumentAppend("{cflags_extra}")
		.argumentAppend("{includedir}").argumentAppend("-shared")
		.argumentAppend("-o")
		.argumentAppend("{target}").argumentAppend("{source}")
		.argumentAppend("{dependencies}");

	m_libcompile.argumentsClear();
	m_libcompile.nameSet("ar").argumentAppend("rcs").argumentAppend("{target}")
		.argumentAppend("{dependencies}");

	m_versionquery.argumentsClear();
	m_versionquery.nameSet("g++").argumentAppend("-E").argumentAppend("-dM")
		.argumentAppend("-x").argumentAppend("c++").argumentAppend("{nullfile}");

	m_pkgconfig.argumentsClear();
	m_pkgconfig.nameSet("pkg-config");
	m_pkgconfig.argumentAppend("{action}").argumentAppend("{libname}");

	return *this;
	}

void TargetCxxOptions::configDump(ResourceObject& cxxoptions) const
	{
		{
		ResourceObject objcompile(ResourceObject::Type::OBJECT);
		m_objcompile.configDump(objcompile);
		cxxoptions.objectSet("objcompile",std::move(objcompile));
		}

		{
		ResourceObject appcompile(ResourceObject::Type::OBJECT);
		m_appcompile.configDump(appcompile);
		cxxoptions.objectSet("appcompile",std::move(appcompile));
		}

		{
		ResourceObject dllcompile(ResourceObject::Type::OBJECT);
		m_dllcompile.configDump(dllcompile);
		cxxoptions.objectSet("dllcompile",std::move(dllcompile));
		}

		{
		ResourceObject libcompile(ResourceObject::Type::OBJECT);
		m_libcompile.configDump(libcompile);
		cxxoptions.objectSet("libcompile",std::move(libcompile));
		}

		{
		ResourceObject versionquery(ResourceObject::Type::OBJECT);
		m_versionquery.configDump(versionquery);
		cxxoptions.objectSet("versionquery",std::move(versionquery));
		}

		{
		ResourceObject pkgconfig(ResourceObject::Type::OBJECT);
		m_pkgconfig.configDump(pkgconfig);
		cxxoptions.objectSet("pkgconfig",std::move(pkgconfig));
		}

	cxxoptions.objectSet("libdir_format",ResourceObject(m_libdir_format.c_str()))
		.objectSet("includedir_format",ResourceObject(m_includedir_format.c_str()))
		.objectSet("libext_format",ResourceObject(m_libext_format.c_str()))
		.objectSet("libint_format",ResourceObject(m_libint_format.c_str()))
		.objectSet("stdprefix",ResourceObject(m_stdprefix.c_str()))
		.objectSet("cxxversion_max",ResourceObject(static_cast<long long int>(m_cxxversion_max)))
		.objectSet("cxxversion_min",ResourceObject(static_cast<long long int>(m_cxxversion_min)))
		.objectSet("cflags_format",ResourceObject(m_cflags_format.c_str()));

		{
		ResourceObject includedir(ResourceObject::Type::ARRAY);
		stringArrayGet(includedir,m_includedir);
		cxxoptions.objectSet("includedir",std::move(includedir));
		}

		{
		ResourceObject includedir_noscan(ResourceObject::Type::ARRAY);
		stringArrayGet(includedir_noscan,m_includedir_noscan);
		cxxoptions.objectSet("includedir_noscan",std::move(includedir_noscan));
		}

		{
		ResourceObject libdir(ResourceObject::Type::ARRAY);
		stringArrayGet(libdir,m_libdir);
		cxxoptions.objectSet("libdir",std::move(libdir));
		}

		{
		ResourceObject cflags_extra(ResourceObject::Type::ARRAY);
		stringArrayGet(cflags_extra,m_cflags_extra);
		cxxoptions.objectSet("cflags_extra",std::move(cflags_extra));
		}
	}

TargetCxxOptions& TargetCxxOptions::cflagsExtraAppend(const char* flagname)
	{
	m_cflags_extra.push_back(std::string(flagname));
	return *this;
	}

TargetCxxOptions& TargetCxxOptions::libdirAppend(const char* dir)
	{
	m_libdir.push_back(std::string(dir));
	return *this;
	}
