//@	{
//@	 "targets":[{"name":"targetcxxcompiler.o","type":"object"}]
//@	}

#include "targetcxxcompiler.hpp"
#include "dependency.hpp"
#include "fileutils.hpp"

using namespace Maike;

TargetCxxCompiler::TargetCxxCompiler(const TargetCxxOptions& options):
	r_options(options)
	{}
