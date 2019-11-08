//@	{"targets":[{"name":"targettrivial.o","type":"object"}]}

#include "targettrivial.hpp"
#include "../resourceobject.hpp"
#include "../fileutils.hpp"
#include "../writebuffer.hpp"

using namespace Maike;

TargetTrivial::TargetTrivial(const ResourceObject& obj
	,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count):
	TargetBase(obj,name_src,in_dir,root,id,line_count)
	{
	m_src=dircat(in_dir,dircat( static_cast<const char*>(obj.objectGet("srcdir"))
		,static_cast<const char*>(obj.objectGet("name")) ));
	dependencyAdd(Dependency(m_src.c_str(),Dependency::Relation::MISC));
	}

void TargetTrivial::dumpDetails(ResourceObject&) const
	{
	}

bool TargetTrivial::upToDate(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir) const
	{
	auto name_out=dircat(target_dir,nameGet());

	if(FileUtils::newer(sourceNameGet(),name_out.c_str()))
		{return 0;}

	auto up_to_date=[&name_out](const char* name,Dependency::Relation)
		{return !FileUtils::newer(name,name_out.c_str());};

	if(!dependenciesProcess(target_dir,dependencies(),USE_MISC,up_to_date))
		{return 0;}

	return 1;
	}


void TargetTrivial::compileImpl(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir)
	{
	auto name_out=dircat(target_dir,nameGet());

	auto cp=[&name_out](const char* name,Dependency::Relation)
		{
		FileUtils::copy(name,name_out.c_str());
		return true;
		};
	dependenciesProcess(target_dir,dependencies(),USE_MISC,cp);
	}

TargetTrivial::~TargetTrivial() noexcept
	{
	}


TargetTrivial* TargetTrivial::create(const ResourceObject& obj,const char* name_src
	,const char* in_dir,const char* root,size_t id,size_t line_count)
	{return new TargetTrivial(obj,name_src,in_dir,root,id,line_count);}

void TargetTrivial::destroy() noexcept
	{delete this;}
