//@	{"targets":[{"name":"targetcxxloader.o","type":"object"}]}

#include "targetcxxloader.hpp"
#include <cstdio>

using namespace Maike;

void TargetCxxLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph) const
	{
	printf("Loading targets from %s/%s\n",in_dir,name_src);
	}
