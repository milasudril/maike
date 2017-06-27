//@	{"targets":[{"name":"targetcp.o","type":"object"}]}

#include "targetcp.hpp"
#include "../resourceobject.hpp"
#include "../fileutils.hpp"
#include "../writebuffer.hpp"

using namespace Maike;

TargetCP::TargetCP(const ResourceObject& obj
	,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	{
	}

void TargetCP::dumpDetails(ResourceObject&) const
	{
	}

bool TargetCP::upToDate(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir) const
	{
	auto name_out=dircat(target_dir,nameGet());
	
	if(FileUtils::newer(sourceNameGet(),name_out.c_str()))
		{return 0;}

	return 1;
	}


void TargetCP::compileImpl(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir)
	{
	auto name_out=dircat(target_dir,nameGet());
	FileUtils::copy(sourceNameGet(),name_out.c_str());
	}

TargetCP::~TargetCP() noexcept
	{
	}


TargetCP* TargetCP::create(const ResourceObject& obj,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count)
	{return new TargetCP(obj,name_src,in_dir,root,id,line_count);}

void TargetCP::destroy() noexcept
	{delete this;}
