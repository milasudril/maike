//@	{
//@	"targets":[{"name":"dependency.o","type":"object"}]
//@	}

#include "dependency.hpp"
#include "resourceobject.hpp"
#include "stringkey.hpp"
#include "errormessage.hpp"
#include "exceptionhandler.hpp"
#include "variant.hpp"

using namespace Maike;

Dependency::Dependency(const char* name,Relation relation):
	m_name(name),r_target(nullptr),m_rel(relation)
	{}

Dependency::Dependency():r_target(nullptr),m_rel(Relation::LEAF)
	{}

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


Dependency::Dependency(const ResourceObject& obj):
	 m_name(static_cast<const char*>(obj.objectGet("ref"))),r_target(nullptr)
	,m_rel(relation(static_cast<const char*>(obj.objectGet("rel"))))
	{
	}

Dependency::Dependency(const ResourceObject& obj,const char* in_dir):
	 r_target(nullptr)
	,m_rel(relation(static_cast<const char*>(obj.objectGet("rel"))))
	{
	m_name=in_dir;
	m_name+='/';
	m_name+=static_cast<const char*>(obj.objectGet("ref"));
	}

void Dependency::dump(ResourceObject& dependency) const
	{
	dependency.objectSet("ref",ResourceObject(nameGet()))
		.objectSet("rel",ResourceObject(relation(relationGet())));
	}
