//@	{"targets":[{"name":"targetdirectory.o","type":"object"}]}

#include "targetdirectory.hpp"
#include "targetdirectorycompiler.hpp"

using namespace Maike;

void TargetDirectory::compile(Twins<const Dependency*> dependency_list
	,Invoker& invoker)
	{
	r_compiler.compile(*this,invoker);
	}

bool TargetDirectory::upToDate(Twins<const Dependency*> dependency_list
	,Invoker& invoker) const
	{
	return r_compiler.upToDate(*this,invoker);
	}
