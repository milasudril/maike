//@	{"targets":[{"name":"targetcxx.o","type":"object"}]}

#include "targetcxx.hpp"
#include "resourceobject.hpp"
#include "stringkey.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "targetcxxcompiler.hpp"
#include "fileutils.hpp"
#include <algorithm>

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



static std::vector<TargetCxxCompiler::FileInfo> depstringCreate(
	 std::vector<std::string>& strings_temp
	,const char* target_dir
	,Twins<const Dependency*> dependency_list_full)
	{
	std::vector<TargetCxxCompiler::FileInfo> ret;
	while(dependency_list_full.first!=dependency_list_full.second)
		{
		auto target_rel=dynamic_cast<const TargetCxx*>(dependency_list_full.first->target());
		if(target_rel==nullptr)
			{
			if(dependency_list_full.first->relationGet()==Dependency::Relation::EXTERNAL)
				{
				auto t=dependency_list_full.first->target();
				ret.push_back({t->nameGet(),TargetCxxCompiler::FileUsage::LIB_EXTERNAL});
				}
			}
		else
			{
			if(target_rel->typeGet()!=TargetCxx::Type::INCLUDE)
				{
				std::string name_full(target_dir);
				name_full+='/';
				name_full+=target_rel->nameGet();
				strings_temp.push_back(std::move(name_full));
				ret.push_back({strings_temp.back().c_str(),TargetCxxCompiler::FileUsage::NORMAL});
				}
			}
		++dependency_list_full.first;
		}
	return std::move(ret);
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
			std::vector<std::string> strings_temp;
			auto depfiles=depstringCreate(strings_temp,target_dir,dependency_list_full);
			depfiles.push_back({sourceNameGet(),TargetCxxCompiler::FileUsage::NORMAL});

			auto deps_begin=depfiles.data();
			auto deps_end=deps_begin + depfiles.size();
			std::reverse(deps_begin,deps_end);
			r_compiler.compileApplication({deps_begin,deps_end},name_full.c_str(),m_options_extra);
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
	,const char* target_name_full
	,const char* target_dir)
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
				std::string target_rel_name_full(target_dir);
				target_rel_name_full+='/';
				target_rel_name_full+=target_rel->nameGet();
				if(FileUtils::newer(target_rel_name_full.c_str(),target_name_full))
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
				,name_full.c_str()
				,target_dir);
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
