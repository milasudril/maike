//@	{"targets":[{"name":"targetdirectory.o","type":"object"}]}

#include "targetdirectory.hpp"
#include "invoker.hpp"

using namespace Maike;

void TargetDirectory::compile(Twins<const Dependency*> dependency_list
	,Invoker& invoker)
	{
	invoker.mkdir(nameGet());
	}

bool TargetDirectory::upToDate(Twins<const Dependency*> dependency_list
	,Invoker& invoker) const
	{
	return invoker.exists(nameGet());
	}
