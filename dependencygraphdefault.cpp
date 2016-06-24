//@ {
//@ "targets":[{"name":dependencygraphdefault.o","type":"include"}]
//@ }

#include "dependencygraphdefault.hpp"
#include "target.hpp"
#include "dependency.hpp"

using namespace Maike;

DependencyGraphDefault& DependencyGraphDefault::targetRegister(std::unique_ptr<Target>&& target)
	{
	auto name=target->nameGet();
	auto i=m_targets.find(KeyType(name));
	if( i!=m_targets.end() )
		{
		throw __FILE__;
	//TODO:	throw ErrorMessage("#0;: Target #1; has already been defined in #2;"
	//		,{target->nameSourceGet(),name,i->second->nameSourceGet()});
		}
	m_targets.emplace(KeyType(name),std::move(target));
	return *this;
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
			auto dep=deps.first;
			if(dep->relationGet()!=Dependency::Relation::EXTERNAL)
				{
				auto t=m_targets.find(KeyType(dep->nameGet()));
				if(t==i_end)
					{
				//TODO: Here, we should look in our repository before throwing
				// an exception
					throw __FILE__;
				//TODO: throw ErrorMessage("#0; Dependency #1; is not satisfied"
					//	,{i->nameSourceGet(),dep->nameGet()});

					}
				dep->targetSet(*(t->second));
				}
			++(deps.first);
			}
		++i;
		}
	return *this;
	}

Target* DependencyGraphDefault::targetFind(const char* name)
	{
	auto t=m_targets.find(KeyType(name));
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
