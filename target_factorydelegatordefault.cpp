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

#include <cstring>

using namespace Maike;

Target_FactoryDelegatorDefault::Target_FactoryDelegatorDefault(const ExpressionEvaluator& eval)
	:r_eval(eval),m_id_current(0)
	{
	}

Target_FactoryDelegatorDefault& Target_FactoryDelegatorDefault::factoryRegister(const Stringkey& filename_ext
	,const Target_Factory& factory)
	{
	m_r_factories[filename_ext]=&factory;
	return *this;
	}

Handle<Target> Target_FactoryDelegatorDefault::targetCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir)
	{
	auto suffix=strrchr(name_src,'.');
	auto i=m_r_factories.find(Stringkey(suffix));
	if(i==m_r_factories.end())
		{
		exceptionRaise(ErrorMessage("#0;: #1; is not associated with any target factory"
			,{name_src,suffix}));
		}
	return i->second->targetCreate(obj,name_src,in_dir,idNext());
	}

Handle<Target> Target_FactoryDelegatorDefault::targetCreate(const ResourceObject& obj
	,const char* in_dir)
	{
	return targetCreate(obj,static_cast<const char*>(obj.objectGet("source_name"))
		,in_dir);
	}




static void targetsCreate(const ResourceObject& targets,const char* name_src
	,const char* in_dir,Target_FactoryDelegator& delegator
	,Target_FactoryDelegator::Callback& cb)
	{
	auto N=targets.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		auto target=targets.objectGet(k);
		if(target.objectExists("source_name" ) || name_src==nullptr)
			{cb(delegator,delegator.targetCreate(target,in_dir));}
		else
			{cb(delegator,delegator.targetCreate(target,name_src,in_dir));}
		}
	}

void Target_FactoryDelegatorDefault::targetsCreate(const ResourceObject& obj
	,const char* name_src,const char* in_dir,Callback&& cb)
	{targetsCreateImpl(obj,name_src,in_dir,cb);}

void Target_FactoryDelegatorDefault::targetsCreate(const ResourceObject& obj
	,const char* in_dir,Callback&& cb)
	{targetsCreateImpl(obj,nullptr,in_dir,cb);}

void Target_FactoryDelegatorDefault::targetsCreateImpl(const ResourceObject& obj
	,const char* name_src,const char* in_dir,Callback& cb)
	{
	if(obj.objectExists("targets"))
		{::targetsCreate(obj.objectGet("targets"),name_src,in_dir,*this,cb);}
	else
		{
		auto N_cases=obj.objectCountGet();
		for(decltype(N_cases) k=0;k<N_cases;++k)
			{
			auto case_obj=obj.objectGet(k);
			if(case_obj.typeGet()==ResourceObject::Type::ARRAY)
				{
				if(case_obj.objectCountGet()!=2)
					{exceptionRaise(ErrorMessage("A condition must have only a condition and a target definition.",{}));}

				auto expression=static_cast<const char*>( case_obj.objectGet(static_cast<size_t>(0)) );
				if(static_cast<int64_t>( r_eval.evaluate(expression) ))
					{
					::targetsCreate(case_obj.objectGet(1).objectGet("targets")
						,name_src,in_dir,*this,cb);
					break;
					}
				}
			else
			if(case_obj.objectExists("targets"))
				{
				::targetsCreate(case_obj.objectGet("targets"),name_src,in_dir
					,*this,cb);
				break;
				}
			}
		}
	}

