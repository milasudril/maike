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
		}
	return nullptr;
	}


Dependency::Dependency(const ResourceObject& obj,const char* root):m_name(nullptr),r_target(nullptr)
	,m_rel(relation(static_cast<const char*>(obj.objectGet("rel"))))
	{
	nameSet(rootStrip(static_cast<const char*>(obj.objectGet("ref")),root).c_str());
	}

Dependency::Dependency(const ResourceObject& obj,const char* in_dir,const char* root):
	 m_name(nullptr),r_target(nullptr)
	,m_rel(relation(static_cast<const char*>(obj.objectGet("rel"))))
	{
	auto name_temp=rootStrip(dircat(in_dir,static_cast<const char*>(obj.objectGet("ref"))),root);
	nameSet(name_temp.c_str(),name_temp.size());
	}

void Dependency::dump(ResourceObject& dependency) const
	{
	dependency.objectSet("ref",ResourceObject(nameGet()))
		.objectSet("rel",ResourceObject(relation(relationGet())));
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
