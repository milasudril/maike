//@	{"targets":[{"name":"targetdirectory.o","type":"object"}]}

#include "targetdirectory.hpp"
#include "invoker.hpp"

using namespace Maike;

void TargetDirectory::compile(Twins<const Dependency*> dependency_list
	,Invoker& invoker)
	{
//TODO invoker.mkdir(nameFullGet());
	}

bool TargetDirectory::upToDate(Twins<const Dependency*> dependency_list
	,Invoker& invoker) const
	{
	return 0;
//TODO return invoker.exists(nameFullGet());
	}
