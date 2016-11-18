//@	{
//@	"targets":[{"name":"dependency.o","type":"object"}]
//@	}

#include "dependency.hpp"
#include "resourceobject.hpp"
#include "stringkey.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "variant.hpp"
#include "pathutils.hpp"
#include <cstdlib>
#include <cstring>

using namespace Maike;

static Dependency::Relation relation(const char* string)
	{
	auto key=Stringkey(string);
	if(key==Stringkey("implementation"))
		{return Dependency::Relation::IMPLEMENTATION;}
	if(key==Stringkey("external"))
		{return Dependency::Relation::EXTERNAL;}
	if(key==Stringkey("internal"))
		{return Dependency::Relation::INTERNAL;}
	if(key==Stringkey("leaf"))
		{return Dependency::Relation::LEAF;}
	if(key==Stringkey("generated"))
		{return Dependency::Relation::GENERATED;}
	if(key==Stringkey("file"))
		{return Dependency::Relation::FILE;}
	if(key==Stringkey("include"))
		{return Dependency::Relation::INCLUDE;}

	exceptionRaise(ErrorMessage("Unknown relation type #0;",{string}));
	}

static const char* relation(Dependency::Relation rel)
	{
	switch(rel)
		{
		case Dependency::Relation::IMPLEMENTATION:
			return "implementation";
		case Dependency::Relation::EXTERNAL:
			return "external";
		case Dependency::Relation::INTERNAL:
			return "internal";
		case Dependency::Relation::LEAF:
			return "leaf";
		case Dependency::Relation::GENERATED:
			return "generated";
		case Dependency::Relation::FILE:
			return "file";
		case Dependency::Relation::INCLUDE:
			return "file";
		}
	return nullptr;
	}

Dependency::Dependency(const ResourceObject& obj,const char* in_dir,const char* root):
	 m_name(nullptr),r_target(nullptr)
	,m_rel(relation(static_cast<const char*>(obj.objectGet("rel"))))
	{
	assert(m_rel!=Relation::INTERNAL && m_rel!=Relation::LEAF);
	if(m_rel==Relation::IMPLEMENTATION || m_rel==Relation::GENERATED 
		|| m_rel==Relation::FILE)
		{
		auto name_temp=rootStrip(dircat(in_dir,static_cast<const char*>(obj.objectGet("ref"))),root);
		nameSet(name_temp.c_str(),name_temp.size());
		}
	else
		{nameSet(rootStrip(static_cast<const char*>(obj.objectGet("ref")),root).c_str());}
	}

void Dependency::dump(ResourceObject& dependency) const
	{
	dependency.objectSet("ref",dependency.create(nameGet()))
		.objectSet("rel",dependency.create(relation(relationGet())));
	}

void Dependency::nameSet(const char* name)
	{
	nameSet(name,strlen(name));
	}

void Dependency::nameSet(const char* name,size_t size)
	{
	auto N=size + 1;
	m_name=static_cast<char*>( realloc(m_name,N*sizeof(N)) );
	if(m_name==NULL)
		{exceptionRaise(ErrorMessage("Out of memory",{}));}
	memcpy(m_name,name,N);
	}

void Dependency::nameFree() noexcept
	{
	if(m_name!=nullptr)
		{
		free(m_name);
		m_name=nullptr;
		}
	}
