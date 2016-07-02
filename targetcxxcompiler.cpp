//@	{
//@	 "targets":[{"name":"targetcxxcompiler.o","type":"object"}]
//@	}

#include "targetcxxcompiler.hpp"
#include "dependency.hpp"
#include "fileutils.hpp"

using namespace Maike;

TargetCxxCompiler::TargetCxxCompiler(const ResourceObject& options):
	m_options(options)
	{
	m_cxxversion_default=m_options.cxxversionDefaultGet();
	}
