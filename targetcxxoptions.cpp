//@	 {"targets":[{"name":"targetcxxoptions.o","type":"object"}]}

#include "targetcxxoptions.hpp"
#include "resourceobject.hpp"
#include "exceptionhandler.hpp"
#include "errormessage.hpp"
#include "variant.hpp"

using namespace Maike;

static void stringArrayBuild(const ResourceObject& array,std::vector< std::string >& ret )
	{
	auto N=array.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{ret.push_back(std::string(static_cast<const char*>(array.objectGet(k))));}
	}

TargetCxxOptions::TargetCxxOptions()
	{
	clear();
	}

TargetCxxOptions::TargetCxxOptions(const ResourceObject& cxxoptions)
	{
	clear();
	configAppend(cxxoptions);
	}

TargetCxxOptions& TargetCxxOptions::configAppend(const ResourceObject& cxxoptions)
	{
	if(cxxoptions.objectExists("includedir"))
		{stringArrayBuild(cxxoptions.objectGet("includedir"),m_includedir);}

	if(cxxoptions.objectExists("libdir"))
		{stringArrayBuild(cxxoptions.objectGet("libdir"),m_libdir);}


	if(cxxoptions.objectExists("cxxversion_min"))
		{
		m_cxxversion_min=std::max( m_cxxversion_min
			,static_cast<long long int>( cxxoptions.objectGet("cxxversion_min") ) );
		}

	if(cxxoptions.objectExists("cxxversion_max"))
		{
		m_cxxversion_min=std::min( m_cxxversion_max
			,static_cast<long long int>( cxxoptions.objectGet("cxxversion_max") ) );
		}

	if(m_cxxversion_max!=0 && m_cxxversion_min!=0
		&& (m_cxxversion_max < m_cxxversion_min))
		{
		exceptionRaise(ErrorMessage("cxxoptions: In consistent C++ version requirements",{}));
		}


	if(cxxoptions.objectExists("stdprefix"))
		{m_stdprefix=std::string( static_cast<const char*>(cxxoptions.objectGet("stdprefix")) );}



	if(cxxoptions.objectExists("includedir_format"))
		{m_includedir_format=std::string(static_cast<const char*>(cxxoptions.objectGet("includedir_format")));}

	if(cxxoptions.objectExists("libdir_format"))
		{m_includedir_format=std::string(static_cast<const char*>(cxxoptions.objectGet("libdir_format")));}

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
	return *this;
	}

void TargetCxxOptions::clear()
	{
	m_cxxversion_min=__cplusplus;
	m_cxxversion_max=0;
	m_includedir.clear();
	m_libdir.clear();

	m_includedir_format=std::string("-I^");
	m_libdir_format=std::string("-L^");
	m_libext_format=std::string("-l^");
	m_libint_format=std::string("-l:^");
	m_stdprefix=std::string("-std=");

	m_objcompile.nameSet("g++").argumentsClear().argumentAppend("-c")
		.argumentAppend("-g").argumentAppend("-fpic")
		.argumentAppend("{cxxversion}").argumentAppend("-Wall")
		.argumentAppend("{includedir}").argumentAppend("-o")
		.argumentAppend("{target}").argumentAppend("{source}");

	m_appcompile.nameSet("g++").argumentsClear().argumentAppend("-g")
		.argumentAppend("-fpic").argumentAppend("{cxxversion}")
		.argumentAppend("-Wall").argumentAppend("{includedir}")
		.argumentAppend("-o").argumentAppend("{target}")
		.argumentAppend("{dependencies}");

	m_dllcompile.nameSet("g++").argumentsClear().argumentAppend("-g")
		.argumentAppend("-fpic").argumentAppend("{cxxversion}")
		.argumentAppend("-Wall").argumentAppend("{includedir}")
		.argumentAppend("-shared").argumentAppend("-o")
		.argumentAppend("{target}").argumentAppend("{dependencies}");

	m_libcompile.nameSet("ar").argumentsClear().argumentAppend("rcs")
		.argumentAppend("{target}").argumentAppend("{dependencies}");

	m_versionquery.nameSet("g++").argumentsClear().argumentAppend("-E")
		.argumentAppend("-dM").argumentAppend("-x").argumentAppend("c++")
		.argumentAppend("{nullfile}");
	}
