//@	{
//@	 "targets":[{"name":"targetcxxcompiler.o","type":"object"}]
//@	}

#include "targetcxxcompiler.hpp"
#include "dependency.hpp"
#include "fileutils.hpp"

using namespace Maike;

TargetCxxCompiler::TargetCxxCompiler(const ResourceObject& options
	,const ParameterSet& params_global):
	m_options(options,params_global)
	{
	m_cxxversion_default=m_options.cxxversionDefaultGet();
	}
