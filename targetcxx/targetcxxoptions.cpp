//@	 {"targets":[{"name":"targetcxxoptions.o","type":"object"}]}

#include "targetcxxoptions.hpp"
#include "../resourceobject.hpp"
#include "../exceptionhandler.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include <limits>

using namespace Maike;

template<class Validator>
static void stringArrayBuild(const ResourceObject& array,std::vector< std::string >& ret
	,Validator&& valid)
	{
	auto N=array.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		auto str=static_cast<const char*>(array.objectGet(k));
		if(valid(str))
			{ret.push_back(std::string(str));}
		}
	}

static void stringArrayGet(ResourceObject& ret,const std::vector< std::string >& array)
	{
	auto ptr=array.data();
	auto ptr_end=ptr + array.size();
	while(ptr!=ptr_end)
		{
		ret.objectAppend(ret.create(ptr->c_str()));
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
		{
		stringArrayBuild(cxxoptions.objectGet("includedir"),m_includedir
			,[this](const char* testval)
				{return m_includedir_dup.insert(Stringkey(testval)).second;}
			);
		}

	if(cxxoptions.objectExists("includedir_noscan"))
		{
		stringArrayBuild(cxxoptions.objectGet("includedir_noscan"),m_includedir_noscan
			,[this](const char* testval)
				{return m_includedir_noscan_dup.insert(Stringkey(testval)).second;}
			);
		}

	if(cxxoptions.objectExists("libdir"))
		{
		stringArrayBuild(cxxoptions.objectGet("libdir"),m_libdir
			,[this](const char* testval)
				{return m_libdir_dup.insert(Stringkey(testval)).second;}
			);
		}

	if(cxxoptions.objectExists("iquote"))
		{
		stringArrayBuild(cxxoptions.objectGet("iquote"),m_iquote
			,[this](const char* testval)
				{return m_iquote_dup.insert(Stringkey(testval)).second;}
			);
		}


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
		{
		stringArrayBuild(cxxoptions.objectGet("cflags_extra"),m_cflags_extra
			,[this](const char* testval)
				{return m_cflags_extra_dup.insert(Stringkey(testval)).second;}
			);
		}



	if(cxxoptions.objectExists("includedir_format"))
		{m_includedir_format=std::string(static_cast<const char*>(cxxoptions.objectGet("includedir_format")));}

	if(cxxoptions.objectExists("libdir_format"))
		{m_libdir_format=std::string(static_cast<const char*>(cxxoptions.objectGet("libdir_format")));}

	if(cxxoptions.objectExists("libext_format"))
		{m_libext_format=std::string(static_cast<const char*>(cxxoptions.objectGet("libext_format")));}

	if(cxxoptions.objectExists("libint_format"))
		{m_libint_format=std::string(static_cast<const char*>(cxxoptions.objectGet("libint_format")));}

	if(cxxoptions.objectExists("iquote_format"))
		{m_iquote_format=std::string(static_cast<const char*>(cxxoptions.objectGet("iquote_format")));}

	if(cxxoptions.objectExists("include_format"))
		{m_include_format=std::string(static_cast<const char*>(cxxoptions.objectGet("include_format")));}

	if(cxxoptions.objectExists("mode"))
		{m_mode=std::string(static_cast<const char*>(cxxoptions.objectGet("mode")));}


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

	if(cxxoptions.objectExists("autorun_launcher"))
		{m_autorun_launcher=Command(cxxoptions.objectGet("autorun_launcher"));}

	return *this;
	}

template<class T,class Validator>
void append(std::vector<T>& a, const std::vector<T>& b,Validator&& valid)
	{
	auto ptr=b.data();
	auto ptr_end=ptr + b.size();
	while(ptr!=ptr_end)
		{
		if(valid(*ptr))
			{a.push_back(*ptr);}
		++ptr;
		}
	}


TargetCxxOptions& TargetCxxOptions::includedirNoscanAppend(const char* dir)
	{
	auto key=Stringkey(dir);
	if(m_includedir_noscan_dup.insert(key).second)
		{m_includedir_noscan.push_back(std::string(dir));}
	return *this;
	}

TargetCxxOptions& TargetCxxOptions::configAppend(const TargetCxxOptions& cxxoptions)
	{
	append(m_includedir,cxxoptions.m_includedir
		,[this](const std::string& testval)
			{return m_includedir_dup.insert(Stringkey(testval.c_str())).second;}
		);
	append(m_includedir_noscan,cxxoptions.m_includedir_noscan
		,[this](const std::string& testval)
			{return m_includedir_noscan_dup.insert(Stringkey(testval.c_str())).second;}
		);
	append(m_libdir,cxxoptions.m_libdir
		,[this](const std::string& testval)
			{return m_libdir_dup.insert(Stringkey(testval.c_str())).second;}
		);
	append(m_cflags_extra,cxxoptions.m_cflags_extra
		,[this](const std::string& testval)
			{return m_cflags_extra_dup.insert(Stringkey(testval.c_str())).second;}
		);
	append(m_iquote,cxxoptions.m_iquote
		,[this](const std::string& testval)
			{return m_iquote_dup.insert(Stringkey(testval.c_str())).second;}
		);

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

	if(cxxoptions.m_iquote_format.size()!=0)
		{m_iquote_format=cxxoptions.m_iquote_format;}

	if(cxxoptions.m_include_format.size()!=0)
		{m_include_format=cxxoptions.m_include_format;}

	if(cxxoptions.m_mode.size()!=0)
		{m_mode=cxxoptions.m_mode;}
	if(m_mode.size()==0)
		{m_mode=std::string("c++");} //make sure there is a valid mode set. #63


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

	if(cxxoptions.m_autorun_launcher)
		{m_autorun_launcher=cxxoptions.m_autorun_launcher;}

	return *this;
	}

void TargetCxxOptions::configClear()
	{
	m_includedir.clear();
	m_includedir_dup.clear();
	m_includedir_noscan.clear();
	m_includedir_noscan_dup.clear();
	m_libdir.clear();
	m_libdir_dup.clear();
	m_cflags_extra.clear();
	m_cflags_extra_dup.clear();
	m_objcompile.nameSet("").argumentsClear();
	m_autorun_launcher.nameSet("").argumentsClear();
	m_dllcompile.nameSet("").argumentsClear();
	m_appcompile.nameSet("").argumentsClear();
	m_libcompile.nameSet("").argumentsClear();
	m_cxxversion_min=0;
	m_cxxversion_max=std::numeric_limits<decltype(m_cxxversion_max)>::max();
	m_iquote.clear();
	m_iquote_dup.clear();
	m_mode.clear(); //we must clear mode #65
	}

TargetCxxOptions& TargetCxxOptions::configAppendDefault()
	{
	m_cxxversion_min=__cplusplus;
	m_cxxversion_max=std::numeric_limits<decltype(m_cxxversion_max)>::max();

	m_includedir_format=std::string("-I^");
	m_libdir_format=std::string("-L^");
	m_libext_format=std::string("-l^");
	m_libint_format=std::string("-l:^");
	m_stdprefix=std::string("-std=");
	m_cflags_format=std::string("-^");
	m_iquote_format=std::string("-iquote^");
	m_include_format=std::string("-include^");

	m_objcompile.argumentsClear();
	m_objcompile.nameSet("g++").argumentAppend("-c")
		.argumentAppend("-g").argumentAppend("-fpic")
		.argumentAppend("{cxxversion}").argumentAppend("-Wall")
		.argumentAppend("{cflags_extra}")
		.argumentAppend("{iquote}")
		.argumentAppend("{includedir}")
		.argumentAppend("-DMAIKE_TARGET_DIRECTORY={target_directory}")
		.argumentAppend("-DMAIKE_CURRENT_DIRECTORY={current_directory}")
		.argumentAppend("{includes_extra}")
		.argumentAppend("-o").argumentAppend("{target}")
		.argumentAppend("{source}");

	m_appcompile.argumentsClear();
	m_appcompile.nameSet("g++").argumentAppend("-g")
		.argumentAppend("-fpic").argumentAppend("{cxxversion}")
		.argumentAppend("-Wall").argumentAppend("{cflags_extra}")
		.argumentAppend("{iquote}").argumentAppend("{includedir}")
		.argumentAppend("-DMAIKE_TARGET_DIRECTORY={target_directory}")
		.argumentAppend("-DMAIKE_CURRENT_DIRECTORY={current_directory}")
		.argumentAppend("{includes_extra}")
		.argumentAppend("-o").argumentAppend("{target}")
		.argumentAppend("{source}")
		.argumentAppend("{libdir}")
		.argumentAppend("{dependencies}");

	m_dllcompile.argumentsClear();
	m_dllcompile.nameSet("g++").argumentAppend("-g")
		.argumentAppend("-fpic").argumentAppend("{cxxversion}")
		.argumentAppend("-Wall").argumentAppend("{cflags_extra}")
		.argumentAppend("{iquote}").argumentAppend("{includedir}")
		.argumentAppend("-DMAIKE_TARGET_DIRECTORY={target_directory}")
		.argumentAppend("-DMAIKE_CURRENT_DIRECTORY={current_directory}")
		.argumentAppend("{includes_extra}")
		.argumentAppend("-shared")
		.argumentAppend("-o")
		.argumentAppend("{target}").argumentAppend("{source}")
		.argumentAppend("{libdir}")
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

	iquoteAppend(".");
	m_mode=std::string("c++");

	return *this;
	}

void TargetCxxOptions::configDump(ResourceObject& cxxoptions) const
	{
		{
		auto objcompile=cxxoptions.createObject();
		m_objcompile.configDump(objcompile);
		cxxoptions.objectSet("objcompile",std::move(objcompile));
		}

		{
		auto appcompile=cxxoptions.createObject();
		m_appcompile.configDump(appcompile);
		cxxoptions.objectSet("appcompile",std::move(appcompile));
		}

		{
		auto dllcompile=cxxoptions.createObject();
		m_dllcompile.configDump(dllcompile);
		cxxoptions.objectSet("dllcompile",std::move(dllcompile));
		}

		{
		auto libcompile=cxxoptions.createObject();
		m_libcompile.configDump(libcompile);
		cxxoptions.objectSet("libcompile",std::move(libcompile));
		}

		{
		auto versionquery=cxxoptions.createObject();
		m_versionquery.configDump(versionquery);
		cxxoptions.objectSet("versionquery",std::move(versionquery));
		}

		{
		auto pkgconfig=cxxoptions.createObject();
		m_pkgconfig.configDump(pkgconfig);
		cxxoptions.objectSet("pkgconfig",std::move(pkgconfig));
		}

		{
		auto autorun_launcher=cxxoptions.createObject();
		m_autorun_launcher.configDump(autorun_launcher);
		cxxoptions.objectSet("autorun_launcher",std::move(autorun_launcher));
		}

	cxxoptions.objectSet("libdir_format",cxxoptions.create(m_libdir_format.c_str()))
		.objectSet("includedir_format",cxxoptions.create(m_includedir_format.c_str()))
		.objectSet("libext_format",cxxoptions.create(m_libext_format.c_str()))
		.objectSet("libint_format",cxxoptions.create(m_libint_format.c_str()))
		.objectSet("stdprefix",cxxoptions.create(m_stdprefix.c_str()))
		.objectSet("cxxversion_max",cxxoptions.create(static_cast<long long int>(m_cxxversion_max)))
		.objectSet("cxxversion_min",cxxoptions.create(static_cast<long long int>(m_cxxversion_min)))
		.objectSet("cflags_format",cxxoptions.create(m_cflags_format.c_str()))
		.objectSet("iquote_format",cxxoptions.create(m_iquote_format.c_str()))
		.objectSet("include_format",cxxoptions.create(m_include_format.c_str()))
		.objectSet("mode",cxxoptions.create(m_mode.c_str()));

		{
		auto includedir=cxxoptions.createArray();
		stringArrayGet(includedir,m_includedir);
		cxxoptions.objectSet("includedir",std::move(includedir));
		}

		{
		auto includedir_noscan=cxxoptions.createArray();
		stringArrayGet(includedir_noscan,m_includedir_noscan);
		cxxoptions.objectSet("includedir_noscan",std::move(includedir_noscan));
		}

		{
		auto libdir=cxxoptions.createArray();
		stringArrayGet(libdir,m_libdir);
		cxxoptions.objectSet("libdir",std::move(libdir));
		}

		{
		auto iquote=cxxoptions.createArray();
		stringArrayGet(iquote,m_iquote);
		cxxoptions.objectSet("iquote",std::move(iquote));
		}

		{
		auto cflags_extra=cxxoptions.createArray();
		stringArrayGet(cflags_extra,m_cflags_extra);
		cxxoptions.objectSet("cflags_extra",std::move(cflags_extra));
		}
	}

TargetCxxOptions& TargetCxxOptions::cflagsExtraAppend(const char* flagname)
	{
	if(m_cflags_extra_dup.insert(Stringkey(flagname)).second)
		{m_cflags_extra.push_back(std::string(flagname));}
	return *this;
	}

TargetCxxOptions& TargetCxxOptions::libdirAppend(const char* dir)
	{
	if(m_libdir_dup.insert(Stringkey(dir)).second)
		{m_libdir.push_back(std::string(dir));}
	return *this;
	}

TargetCxxOptions& TargetCxxOptions::iquoteAppend(const char* dir)
	{
	if(m_iquote_dup.insert(Stringkey(dir)).second)
		{m_iquote.push_back(std::string(dir));}
	return *this;
	}
