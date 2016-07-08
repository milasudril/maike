//@	{
//@	"targets":[{"name":"targetbase.o","type":"object"}]
//@	}

#include "targetbase.hpp"
#include "resourceobject.hpp"
#include <cstring>

using namespace Maike;

TargetBase::TargetBase(const ResourceObject& obj,const char* name_src,const char* in_dir,size_t id):
	m_child_counter(0),m_id(id),m_source_name(name_src)
	{
	m_name=in_dir;
	m_name+='/';
	m_name+=static_cast<const char*>(obj.objectGet("name"));

	if(obj.objectExists("dependencies"))
		{
		auto deps=obj.objectGet("dependencies");
		auto M=deps.objectCountGet();
		for(decltype(M) l=0;l<M;++l)
			{dependencyAdd(Dependency(deps.objectGet(l)));}
		}

	if(*in_dir!='\0')
		{dependencyAdd(Dependency(in_dir,Dependency::Relation::INTERNAL));}
	}


TargetBase::TargetBase(const char* name,const char* name_src,const char* in_dir,size_t id):
	m_child_counter(0),m_id(id),m_name(name),m_source_name(name_src)
	{
	if(*in_dir!='\0')
		{dependencyAdd(Dependency(in_dir,Dependency::Relation::INTERNAL));}
	}

TargetBase& TargetBase::dependencyAdd(Dependency&& dep)
	{
	m_dependencies.push_back(std::move(dep));
	return *this;
	}
