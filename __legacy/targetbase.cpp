//@	{
//@	"targets":[{"name":"targetbase.o","type":"object"}]
//@	}

#include "targetbase.hpp"
#include "resourceobject.hpp"
#include "timedscope.hpp"
#include "pathutils.hpp"
#include <cstring>
#include <limits>

using namespace Maike;

TargetBase::TargetBase(const ResourceObject& obj,
                       const char* name_src,
                       const char* in_dir,
                       const char* root,
                       size_t id,
                       size_t line_count):
   m_child_counter(0),
   m_id(id),
   m_source_name(name_src),
   m_compilation_time(std::numeric_limits<double>::quiet_NaN()),
   m_loc(line_count)
{
	m_name = rootStrip(dircat(in_dir, static_cast<const char*>(obj.objectGet("name"))), root);
	m_in_dir = rootStrip(in_dir, root);

	if(obj.objectExists("dependencies"))
	{
		auto deps = obj.objectGet("dependencies");
		auto M = deps.objectCountGet();
		for(decltype(M) l = 0; l < M; ++l)
		{
			dependencyAdd(Dependency(deps.objectGet(l), in_dir, root));
		}
	}

	if(m_name != m_in_dir && *in_dir != '\0')
	{ dependencyAdd(Dependency(in_dir, root, Dependency::Relation::INTERNAL)); }

	if(obj.objectExists("description"))
	{ m_description = std::string(static_cast<const char*>(obj.objectGet("description"))); }
}


TargetBase::TargetBase(
   const char* name, const char* name_src, const char* in_dir, const char* root, size_t id):
   m_child_counter(0),
   m_id(id),
   m_source_name(name_src),
   m_compilation_time(std::numeric_limits<double>::quiet_NaN()),
   m_loc(0)
{
	m_name = rootStrip(name, root);
	m_in_dir = rootStrip(in_dir, root);
	if(m_name != m_in_dir && *in_dir != '\0')
	{ dependencyAdd(Dependency(in_dir, root, Dependency::Relation::INTERNAL)); }
}


void TargetBase::dump(ResourceObject& target) const
{
	target.objectSet("name", target.create(m_name.c_str()))
	   .objectSet("source", target.create(m_source_name.c_str()))
	   .objectSet("description", target.create(m_description.c_str()));
	auto dependency_array = target.createArray();
	auto deps = dependencies();
	while(deps.first != deps.second)
	{
		auto dep = target.createObject();
		deps.first->dump(dep);
		dependency_array.objectAppend(std::move(dep));
		++deps.first;
	}
	target.objectSet("dependencies", std::move(dependency_array));
	dumpDetails(target);
}

TargetBase& TargetBase::dependencyAdd(Dependency&& dep)
{
	m_dependencies.push_back(std::move(dep));
	return *this;
}

TargetBase& TargetBase::dependencyInverseAdd(Dependency&& dep)
{
	m_deps_inverse.push_back(std::move(dep));
	return *this;
}

void TargetBase::compile(Twins<const Dependency*> dependency_list,
                         Twins<const Dependency*> dependency_list_full,
                         const char* target_dir)
{
	TimedScope scope(m_compilation_time);
	compileImpl(dependency_list, dependency_list_full, target_dir);
}
