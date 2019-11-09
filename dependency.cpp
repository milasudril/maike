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
#include "writebuffer.hpp"
#include "stdstream.hpp"
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
		{
		WriteBuffer(StdStream::error()).write("Warning: The relation `generated` is deprecated. "
			"Use `misc` instead.\n");
		return Dependency::Relation::MISC;
		}
	if(key==Stringkey("include_generated"))
		{
		WriteBuffer(StdStream::error()).write("Warning: The relation `include_generated` is deprecated. "
			"Use `include_extra` instead.\n");
		return Dependency::Relation::INCLUDE_EXTRA;
		}
	if(key==Stringkey("include_extra"))
		{return Dependency::Relation::INCLUDE_EXTRA;}
	if(key==Stringkey("file"))
		{
		WriteBuffer(StdStream::error()).write("Warning: The relation `file` is deprecated. "
			"Use `misc` instead.\n");
		return Dependency::Relation::MISC;
		}
	if(key==Stringkey("include"))
		{return Dependency::Relation::INCLUDE;}
	if(key==Stringkey("tool"))
		{return Dependency::Relation::TOOL;}
	if(key==Stringkey("misc"))
		{return Dependency::Relation::MISC;}
	if(key==Stringkey("runtime"))
		{return Dependency::Relation::RUNTIME;}
	if(key==Stringkey("external_resource"))
		{return Dependency::Relation::EXTERNAL_RESOURCE;}

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
		case Dependency::Relation::MISC:
			return "misc";
		case Dependency::Relation::INCLUDE:
			return "include";
		case Dependency::Relation::TOOL:
			return "tool";
		case Dependency::Relation::INCLUDE_EXTRA:
			return "include_extra";
		case Dependency::Relation::RUNTIME:
			return "runtime";
		case Dependency::Relation::EXTERNAL_RESOURCE:
			return "external_resource";
		}
	return nullptr;
	}

const char* Dependency::relationGetStr() const
	{return relation(relationGet());}

Dependency::Dependency(const ResourceObject& obj,const char* in_dir,const char* root):
	 m_name(nullptr),r_target(nullptr)
	,m_rel(relation(static_cast<const char*>(obj.objectGet("rel"))))
	{
	assert(m_rel!=Relation::INTERNAL && m_rel!=Relation::LEAF);

//	EXTERNAL, TOOL, RUNTIME, and EXTERNAL_RESOURCE refers to stuff outside the
//	source repo. This means that current directory should not be used in this case.
	if(m_rel==Relation::EXTERNAL || m_rel==Relation::TOOL || m_rel==Relation::RUNTIME
		|| m_rel==Relation::EXTERNAL_RESOURCE)
		{nameSet(rootStrip(static_cast<const char*>(obj.objectGet("ref")),root).c_str());}
	else
		{
		auto name_temp=rootStrip(dircat(in_dir,static_cast<const char*>(obj.objectGet("ref"))),root);
		nameSet(name_temp.c_str(),name_temp.size());
		}
	}

Dependency::Dependency(const ResourceObject& obj):
	 m_name(nullptr), r_target(nullptr)
	,m_rel(relation(static_cast<const char*>(obj.objectGet("rel"))))
	{
	nameSet(static_cast<const char*>(obj.objectGet("ref")));
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
