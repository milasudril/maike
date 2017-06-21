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
#include "resourceobjectjansson.hpp"

#include <cstring>

#include <cstdio>

using namespace Maike;

Target& Target_FactoryDelegatorDefault::dependencyResolve(DependencyGraph& graph
	,const char* target_from,const Dependency& dependency)
	{
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
			TargetPlaceholder::create(name,name,rootGet(),idGet(),Dependency::Relation::MISC));
		auto ret=target.get();
		graph.targetRegister(std::move(target));
		return *ret;
		}

//	It is not. Is it an external dependency?
	if(dependency.relationGet()==Dependency::Relation::EXTERNAL
		|| dependency.relationGet()==Dependency::Relation::TOOL)
		{
		Handle<TargetPlaceholder> target(
			TargetPlaceholder::create(name,name,rootGet(),idGet(),dependency.relationGet()));
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

static bool backrefIs(const char* depname,const ResourceObject& targets,const char* in_dir
	,const char* root)
	{
	auto M=targets.objectCountGet();
	for(decltype(M) l=0;l<M;++l)
		{
		auto target=targets.objectGet(l);
		auto target_name=rootStrip(
			 dircat(in_dir,static_cast<const char*>(target.objectGet("name")))
			,root);
		if(target_name==depname)
			{return 1;}
		}
	return 0;
	}

static std::vector<Dependency> dependenciesCollect(
	Target_FactoryDelegator& delegator
	,Target_FactoryDelegator::DependencyCollector& cb
	,const ResourceObject& targets,const char* in_dir)
	{
	std::vector<Dependency> ret;
		{
		Dependency dep;
		while(cb(delegator,dep,ResourceObjectJansson::createImpl))
			{
			if(!backrefIs(dep.nameGet(),targets,in_dir,delegator.rootGet()))
				{ret.push_back(dep);}
			}
		}
	return std::move(ret);
	}

static void dependenciesAdd(Handle<Target>& target,const std::vector<Dependency>& deps)
	{
	auto dep_first=deps.data();
	auto deps_end=dep_first + deps.size();
	while(dep_first!=deps_end)
		{
		Dependency dep(*dep_first);
		target->dependencyAdd(std::move(dep));
		++dep_first;
		}
	}

static void targetsCreate(const ResourceObject& targets,const char* name_src
	,const char* in_dir,size_t line_count
	,Target_FactoryDelegator& delegator
	,Target_FactoryDelegator::DependencyCollector& cb
	,DependencyGraph& graph)
	{
	auto deps=dependenciesCollect(delegator,cb,targets,in_dir);


	auto N=targets.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		auto target=targets.objectGet(k);
		if(target.objectExists("source_name") || name_src==nullptr)
			{
			auto t=delegator.targetCreate(target,in_dir,0);
			dependenciesAdd(t,deps);
			graph.targetRegister(std::move(t));
			}
		else
			{
			auto t=delegator.targetCreate(target,name_src,in_dir,line_count);
			dependenciesAdd(t,deps);
			graph.targetRegister(std::move(t));
			}
		}
	}

void Target_FactoryDelegatorDefault::targetsCreate(TagExtractor&& extractor
	,const char* name_src,const char* in_dir,DependencyCollector&& cb
	,DependencyGraph& graph)
	{targetsCreateImpl(extractor,name_src,in_dir,cb,graph);}

void Target_FactoryDelegatorDefault::targetsCreate(TagExtractor&& extractor
	,const char* in_dir,DependencyCollector&& cb,DependencyGraph& graph)
	{targetsCreateImpl(extractor,nullptr,in_dir,cb,graph);}

void Target_FactoryDelegatorDefault::targetsCreateImpl(TagExtractor& extractor
	,const char* name_src,const char* in_dir,DependencyCollector& cb
	,DependencyGraph& graph)
	{
	ResourceObjectJansson obj(extractor);
	auto line_count=extractor.linesCountGet();

	if(obj.objectExists("targets"))
		{::targetsCreate(obj.objectGet("targets"),name_src,in_dir,line_count,*this,cb,graph);}
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
						,name_src,in_dir,line_count,*this,cb,graph);
					break;
					}
				}
			else
			if(case_obj.objectExists("targets"))
				{
				::targetsCreate(case_obj.objectGet("targets"),name_src,in_dir
					,line_count,*this,cb,graph);
				break;
				}
			}
		}
	}

void Target_FactoryDelegatorDefault::factoriesUnregister() noexcept
	{
	m_r_factories.clear();
	}

