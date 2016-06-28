//@	{"targets":[{"name":"targetcxx.o","type":"object"}]}

#include "targetcxx.hpp"

using namespace Maike;

TargetCxx::TargetCxx(const ResourceObject& obj,const char* name_src
	,const char* in_dir,size_t id):TargetBase(obj,name_src,in_dir,id)
	{
	}

void TargetCxx::compile(Twins<const Dependency*> dependency_list
	,Invoker& invoker,const char* target_dir)
	{
	}

bool TargetCxx::upToDate(Twins<const Dependency*> dependency_list
	,Invoker& invoker,const char* target_dir) const
	{
	return 0;
	}
