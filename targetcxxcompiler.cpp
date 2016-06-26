//@	{
//@	 "targets":[{"name":"targetcxxcompiler.o","type":"object"}]
//@	}

#include "targetcxxcompiler.hpp"
#include "invoker.hpp"
#include "dependency.hpp"

using namespace Maike;

TargetCxxCompiler& TargetCxxCompiler::directoryTargetsSet(const char* dirname)
	{
	m_dir_targets=dirname;
	return *this;
	}

#endif


