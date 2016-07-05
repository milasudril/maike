//@	{"targets":[{"name":"targetcxx.o","type":"object"}]}

#include "targetcxx.hpp"
#include "resourceobject.hpp"
#include "stringkey.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "targetcxxcompiler.hpp"
#include "fileutils.hpp"

using namespace Maike;

TargetCxx::TargetCxx(const ResourceObject& obj,const TargetCxxCompiler& compiler
	,const char* name_src,const char* in_dir,size_t id):TargetBase(obj,name_src,in_dir,id)
	,r_compiler(compiler)
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

	if(obj.objectExists("cxxoptions"))
		{
		m_options_extra=TargetCxxOptions(obj.objectGet("cxxoptions"));
		}
	}

static std::vector<std::string> depstringCreate(const char* target_dir
	,Twins<const Dependency*> dependency_list_full)
	{
	std::vector<std::string> ret;
	while(dependency_list_full.first!=dependency_list_full.second)
		{
		auto target_rel=dynamic_cast<const TargetCxx*>(dependency_list_full.first->target());
		if(target_rel==nullptr)
			{
			}
		else
			{
			if(target_rel->typeGet()!=TargetCxx::Type::INCLUDE)
				{
				std::string name_full(target_dir);
				name_full+='/';
				name_full+=target_rel->nameGet();
			//	printf("%s\n",target_rel->nameGet());
				ret.push_back(std::move(name_full));
				}
			}
		++dependency_list_full.first;
		}
	return ret;
	}

static std::vector<const char*> cstr_rev(Twins<const std::string*> strings)
	{
	std::vector<const char*> ret;
	while(strings.first!=strings.second)
		{
		--strings.second;
		ret.push_back(strings.second->c_str());

		}
	return ret;
	}


void TargetCxx::compile(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir)
	{
	std::string name_full( target_dir );
	name_full+='/';
	name_full+=nameGet();

	switch(m_type)
		{
		case Type::OBJECT:
			r_compiler.compileObject(sourceNameGet(),name_full.c_str(),m_options_extra);
			break;
		case Type::APPLICATION:
			{
			auto depstring=depstringCreate(target_dir,dependency_list_full);
			depstring.push_back(sourceNameGet());

			auto deps_cstr=cstr_rev({depstring.data(),depstring.data() + depstring.size()});
			fprintf(stderr,"Number of files: %zu\n",depstring.size());
			r_compiler.compileApplication(
				{deps_cstr.data(),deps_cstr.data() + deps_cstr.size()}
				,name_full.c_str()
				,m_options_extra);
			}
			break;
		case Type::INCLUDE:
			break;
		default:
			printf("%s not handled\n",name_full.c_str());
		}
	}


static bool objectUpToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_name_full)
	{
	while(dependency_list.first!=dependency_list.second)
		{
		if(FileUtils::newer(dependency_list.first->nameGet()
			,target_name_full))
			{return 0;}
		++dependency_list.first;
		}
	return 1;
	}

static bool applicationUpToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_name_full)
	{
	while(dependency_list.first!=dependency_list.second)
		{
		if(dependency_list.first->relationGet()!=Dependency::Relation::EXTERNAL)
			{
			if(FileUtils::newer(dependency_list.first->nameGet()
				,target_name_full))
				{return 0;}
			}
		++dependency_list.first;
		}

	while(dependency_list_full.first!=dependency_list_full.second)
		{
		auto target_rel=dynamic_cast<const TargetCxx*>(dependency_list_full.first->target());
		if(target_rel!=nullptr)
			{
			if(target_rel->typeGet()!=TargetCxx::Type::INCLUDE)
				{
				if(FileUtils::newer(target_rel->nameGet(),target_name_full))
					{return 0;}
				}
			}
		++dependency_list_full.first;
		}
	return 1;
	}

bool TargetCxx::upToDate(Twins<const Dependency*> dependency_list
	,Twins<const Dependency*> dependency_list_full
	,const char* target_dir) const
	{
	std::string name_full( target_dir );
	name_full+='/';
	name_full+=nameGet();

	if(FileUtils::newer(sourceNameGet(),name_full.c_str()))
		{return 0;}

	switch(m_type)
		{
		case Type::OBJECT:
			return objectUpToDate(dependency_list,dependency_list_full
				,name_full.c_str());
		case Type::INCLUDE:
			return 1;

		case Type::APPLICATION:
			return applicationUpToDate(dependency_list,dependency_list_full
				,name_full.c_str());
		default:
			break;
		}

	return 1;
	}

TargetCxx* TargetCxx::create(const ResourceObject& obj
	,const TargetCxxCompiler& compiler,const char* name_src
	,const char* in_dir,size_t id)
	{
	return new TargetCxx(obj,compiler,name_src,in_dir,id);
	}

void TargetCxx::destroy() noexcept
	{
	delete this;
	}
