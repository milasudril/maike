//@	{"targets":[{"name":"targetcxx.o","type":"object"}]}

#include "targetcxx.hpp"
using namespace Maike;

TargetCxx::TargetCxx(const char* name,const char* name_src,const char* in_dir
	,size_t id,Type type):TargetBase(name,name_src,in_dir,id)
	{
	}

TargetCxx::~TargetCxx()
	{
	}

