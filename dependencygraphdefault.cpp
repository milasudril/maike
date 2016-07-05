//@	{"targets":[{"name":"dependencygraphdefault.o","type":"object"}]}

#include "dependencygraphdefault.hpp"
#include "target.hpp"
#include "dependency.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "targetplaceholder.hpp"
#include "handle.hpp"

using namespace Maike;

DependencyGraphDefault& DependencyGraphDefault::targetRegister(Handle<Target>&& target)
	{
	auto name=target->nameGet();
	auto key=Stringkey(name);
	auto i=m_targets.find(key);

	if( i!=m_targets.end() )
		{
		exceptionRaise( ErrorMessage("#0;: Target #1; has already been defined in #2;"
			,{target->sourceNameGet(),name,i->second->sourceNameGet()}));
		}
	m_targets.emplace(key,std::move(target));
	return *this;
	}

static Target& dependencyResolve(std::map< Stringkey,Handle<Target> >& targets
	,const char* from
	,const Dependency& dependency)
	{
	auto name=dependency.nameGet();
	auto key=Stringkey(name);
	auto i=targets.find(key);
	if(i!=targets.end()) //Is the target represented in the graph?
		{return *(i->second);}

//	It is not. Is this an external relation?
	if(dependency.relationGet()==Dependency::Relation::EXTERNAL)
		{
	//	Create a placeholder for an external target
		Handle<TargetPlaceholder> target(
			TargetPlaceholder::create(name,name,targets.size()));
		return *(targets.emplace(key,std::move(target)).first)->second;
		}

//	It is not an external relation. Conclude that the dependency is not satisfied
	exceptionRaise(ErrorMessage("#0;: Dependency #1; is not satisfied"
		,{from,name}));
	}

DependencyGraphDefault& DependencyGraphDefault::targetsPatch()
	{
	auto i=m_targets.begin();
	auto i_end=m_targets.end();
	while(i!=i_end)
		{
		auto deps=i->second->dependencies();
		while(deps.first!=deps.second)
			{
			deps.first->targetSet(
				dependencyResolve(m_targets,i->second->nameGet(),*deps.first));
			++(deps.first);
			}
		++i;
		}
	return *this;
	}

Target* DependencyGraphDefault::targetFind(const Stringkey& name)
	{
	auto t=m_targets.find(name);
	if(t==m_targets.end())
		{
		return nullptr;
		}
	return t->second.get();
	}

DependencyGraphDefault& DependencyGraphDefault::targetsProcess(TargetProcessor&& proc)
	{
	auto i=m_targets.begin();
	auto i_end=m_targets.end();
	while(i!=i_end)
		{
		proc(*this,*(i->second));
		++i;
		}
	return *this;
	}
