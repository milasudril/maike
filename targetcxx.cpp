//@	{"targets":[{"name":"targetcxx.o","type":"object"}]}

#include "targetcxx.hpp"
#include "resourceobject.hpp"
#include "stringkey.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"

using namespace Maike;

TargetCxx::TargetCxx(const ResourceObject& obj,const char* name_src
	,const char* in_dir,size_t id):TargetBase(obj,name_src,in_dir,id)
	{
	auto type_string=static_cast<const char*>( obj.objectGet("type") );
	auto key=Stringkey(type_string);
	if(key==Stringkey("object"))
		{m_type=Type::OBJECT;}
	else
	if(key==Stringkey("include"))
		{m_type=Type::INCLUDE;}
	else
	if(key==Stringkey("application"))
		{m_type=Type::APPLICATION;}
	else
	if(key==Stringkey("lib_dynamic"))
		{m_type=Type::LIB_DYNAMIC;}
	else
	if(key==Stringkey("lib_static"))
		{m_type=Type::LIB_STATIC;}
	else
		{
		exceptionRaise(ErrorMessage("#0;: Unknown target type #1;."
			,{name_src,type_string}));
		}
	}

void TargetCxx::compile(Twins<const Dependency*> dependency_list
	,Invoker& invoker,const char* target_dir)
	{
	}

bool TargetCxx::upToDate(Twins<const Dependency*> dependency_list
	,Invoker& invoker,const char* target_dir) const
	{
	return 0;
	}

TargetCxx* TargetCxx::create(const ResourceObject& obj,const char* name_src
	,const char* in_dir,size_t id)
	{
	return new TargetCxx(obj,name_src,in_dir,id);
	}

void TargetCxx::destroy() noexcept
	{
	delete this;
	}
