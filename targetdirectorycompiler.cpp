//@	{"targets":["name":"targetdirectorycompiler.o","type":"object"]}

#include "targetdirectorycompiler.hpp"
#include "target.hpp"
#include "invoker.hpp"
#include <cstring>

using namespace Maike;


TargetDirectoryCompiler& TargetDirectoryCompiler::directoryTargetsSet(const char* dirname)
	{
	m_dir_targets=dirname;
	return *this;
	}

void TargetDirectoryCompiler::compile(Target& target,Invoker& invoker) const
	{
	auto name_full=m_dir_targets;
	if(strcmp(target.nameGet(),".")!=0)
		{
		name_full+='/';
		name_full+=target.nameGet();
		}
	invoker.mkdir(name_full.c_str());
	}

bool TargetDirectoryCompiler::upToDate(const Target& target,Invoker& invoker) const
	{
	auto name_full=m_dir_targets;
	if(strcmp(target.nameGet(),".")!=0)
		{
		name_full+='/';
		name_full+=target.nameGet();
		}
	return invoker.exists(name_full.c_str());
	}
