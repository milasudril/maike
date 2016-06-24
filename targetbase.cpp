//@	{
//@	"targets":[{"name":"targetbase.o","type":"object"}]
//@	}

#include "targetbase.hpp"

using namespace Maike;

TargetBase::TargetBase(const char* name,const char* name_src,const char* in_dir,size_t id):
	m_id(id),m_name(name),m_source_name(name_src)
	{
	if(*in_dir!='\0')
		{dependencyAdd(Dependency(in_dir,Dependency::Relation::INTERNAL));}
	}

TargetBase& TargetBase::dependencyAdd(Dependency&& dep)
	{
	m_dependencies.push_back(std::move(dep));
	return *this;
	}
