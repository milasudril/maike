//@	{"targets":[{"name":"target_factorydelegatordefault.o","type":"object"}]}

#include "target_factorydelegatordefault.hpp"
#include "resourceobject.hpp"
#include "exceptionhandler.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "stringkey.hpp"
#include "target_factory.hpp"
#include "target.hpp"
#include "expressionevaluator.hpp"
#include "dependency.hpp"
#include "fileutils.hpp"
#include "targetplaceholder.hpp"

#include <cstring>

#include <cstdio>

using namespace Maike;

Target& Target_FactoryDelegatorDefault::dependencyResolve(DependencyGraph& graph
	,const char* target_from,const Dependency& dependency)
	{
//TODO: Strip root from dependency name
	auto name=dependency.nameGet();
	auto key=Stringkey(name);

//	Is the target represented in the graph?
	auto ptr_ret=graph.targetFind(key);
	if(ptr_ret!=nullptr)
		{return *ptr_ret;}

//	It is not. Is it an existing file?
	if(FileUtils::exists(name))
		{
		Handle<TargetPlaceholder> target(
			TargetPlaceholder::create(name,name,rootGet(),idGet()));
		auto ret=target.get();
		graph.targetRegister(std::move(target));
		return *ret;
		}

//	It does not. Is it an external dependency?
	if(dependency.relationGet()==Dependency::Relation::EXTERNAL)
		{
		Handle<TargetPlaceholder> target(
			TargetPlaceholder::create(name,name,rootGet(),idGet()));
		auto ret=target.get();
		graph.targetRegister(std::move(target));
		return *ret;
		}

//	Everything failed. Conclude that the dependency is not satisfied
	exceptionRaise(ErrorMessage("#0;: Dependency #1; is not satisfied"
		,{target_from,name}));
	}

Target_FactoryDelegatorDefault::Target_FactoryDelegatorDefault(const ExpressionEvaluator& eval)
	:r_eval(eval)
	{
	}

Target_FactoryDelegatorDefault& Target_FactoryDelegatorDefault::factoryRegister(const Stringkey& filename_ext
	,const Target_Factory& factory)
	{
	m_r_factories[filename_ext]=&factory;
	return *this;
	}

Handle<Target> Target_FactoryDelegatorDefault::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,size_t line_count)
	{
	auto suffix=strrchr(name_src,'.');
	if(suffix==NULL)
		{
		exceptionRaise(ErrorMessage("#0;: It is impossible to choose a target factory "
			"without filename extension"
			,{name_src}));
		}
	auto i=m_r_factories.find(Stringkey(suffix));
	if(i==m_r_factories.end())
		{
		exceptionRaise(ErrorMessage("#0;: #1; is not associated with any target factory"
			,{name_src,suffix}));
		}
//TODO: Look up target name, and strip root here?
	return i->second->targetCreate(obj,name_src,in_dir,rootGet(),idGet(),line_count);
	}


static bool sourceGeneratedIs(const ResourceObject& obj)
	{
	if(obj.objectExists("source_generated"))
		{
		return static_cast<long long int>(obj.objectGet("source_generated"));
		}
	return 0;
	}

Handle<Target> Target_FactoryDelegatorDefault::targetCreate(const ResourceObject& obj
	,const char* in_dir,size_t line_count)
	{
	std::string source_name;
	auto source_generated=sourceGeneratedIs(obj);
	auto source_name_raw=static_cast<const char*>(obj.objectGet("source_name"));

	if(source_generated)
		{
		source_name+=static_cast<const char*>(r_eval.variableGet(Stringkey("target_directory")));
		source_name+='/';
		}

	source_name+=source_name_raw;
	auto ret=targetCreate(obj,source_name.c_str(),in_dir,line_count);
	if(source_generated)
		{
		ret->dependencyAdd(Dependency(source_name_raw,rootGet(),Dependency::Relation::INTERNAL));
		}
	return ret;
	}




static void targetsCreate(const ResourceObject& targets,const char* name_src
	,const char* in_dir,size_t line_count
	,Target_FactoryDelegator& delegator
	,Target_FactoryDelegator::Callback& cb)
	{
	auto N=targets.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		auto target=targets.objectGet(k);
		if(target.objectExists("source_name") || name_src==nullptr)
			{cb(delegator,delegator.targetCreate(target,in_dir,0));}
		else
			{cb(delegator,delegator.targetCreate(target,name_src,in_dir,line_count));}
		}
	}

void Target_FactoryDelegatorDefault::targetsCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,size_t line_count,Callback&& cb)
	{targetsCreateImpl(obj,name_src,in_dir,line_count,cb);}

void Target_FactoryDelegatorDefault::targetsCreate(const ResourceObject& obj
	,const char* in_dir,size_t line_count,Callback&& cb)
	{targetsCreateImpl(obj,nullptr,in_dir,line_count,cb);}

void Target_FactoryDelegatorDefault::targetsCreateImpl(const ResourceObject& obj
	,const char* name_src,const char* in_dir,size_t line_count,Callback& cb)
	{
	if(obj.objectExists("targets"))
		{::targetsCreate(obj.objectGet("targets"),name_src,in_dir,line_count,*this,cb);}
	else
		{
		auto N_cases=obj.objectCountGet();
		for(decltype(N_cases) k=0;k<N_cases;++k)
			{
			auto case_obj=obj.objectGet(k);
			if(case_obj.typeGet()==ResourceObject::Type::ARRAY)
				{
				if(case_obj.objectCountGet()!=2)
					{exceptionRaise(ErrorMessage("#0;: A condition must have only a condition and a target definition.",{name_src}));}

				auto expression=static_cast<const char*>( case_obj.objectGet(static_cast<size_t>(0)) );
				if(static_cast<int64_t>( r_eval.evaluate(expression) ))
					{
					::targetsCreate(case_obj.objectGet(1).objectGet("targets")
						,name_src,in_dir,line_count,*this,cb);
					break;
					}
				}
			else
			if(case_obj.objectExists("targets"))
				{
				::targetsCreate(case_obj.objectGet("targets"),name_src,in_dir
					,line_count,*this,cb);
				break;
				}
			}
		}
	}

void Target_FactoryDelegatorDefault::factoriesUnregister() noexcept
	{
	m_r_factories.clear();
	}

