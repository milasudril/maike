//@	{"targets":[{"name":"dependencygraphdefault.o","type":"object"}]}

#include "dependencygraphdefault.hpp"
#include "dependency.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "targetplaceholder.hpp"

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
	m_patch_needed=1;
	return *this;
	}

static Target& dependencyResolve(std::map< Stringkey,Handle<Target> >& targets
	,const char* from
	,const Dependency& dependency
	,IdGenerator<size_t>& id_gen)
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
			TargetPlaceholder::create(name,name,id_gen.idGet()));
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
	if(i==i_end)
		{
		m_id_range={0,0};
		return *this;
		}

	auto t1=i->second.get();
	Twins<size_t> id_range{t1->idGet(),t1->idGet()};
	while(i!=i_end)
		{
		t1=i->second.get();
		id_range.first=std::min(id_range.first,t1->idGet());
		id_range.second=std::max(id_range.second,t1->idGet());
		auto deps=t1->dependencies();
		while(deps.first!=deps.second)
			{
			auto& t2=dependencyResolve(m_targets,t1->nameGet(),*deps.first
				,r_id_gen);
			id_range.first=std::min(id_range.first,t2.idGet());
			id_range.second=std::max(id_range.second,t2.idGet());
			deps.first->targetSet(t2);
			++(deps.first);
			}
		++i;
		}
	m_id_range=id_range;
	m_patch_needed=0;
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

const Target* DependencyGraphDefault::targetFind(const Stringkey& name) const
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
	if(m_patch_needed)
		{targetsPatch();}
	auto i=m_targets.begin();
	auto i_end=m_targets.end();
	while(i!=i_end)
		{
		proc(*this,*(i->second));
		++i;
		}
	return *this;
	}

void DependencyGraphDefault::targetsProcess(TargetProcessorConst&& proc) const
	{
	if(m_patch_needed)
		{const_cast<DependencyGraphDefault*>(this)->targetsPatch();}
	auto i=m_targets.begin();
	auto i_end=m_targets.end();
	while(i!=i_end)
		{
		proc(*this,*(i->second));
		++i;
		}
	}

DependencyGraphDefault& DependencyGraphDefault::targetsRemove(TargetProcessor&& condition)
	{
	if(m_patch_needed)
		{targetsPatch();}
	auto i=m_targets.begin();
	auto i_end=m_targets.end();
	while(i!=i_end)
		{
		if(condition(*this,*(i->second))==0)
			{
			r_id_gen.idRelease(i->second->idGet());
			m_targets.erase(i);
			}
		++i;
		}
	m_patch_needed=1;
	return *this;
	}

DependencyGraphDefault& DependencyGraphDefault::clear() noexcept
	{
	m_targets.clear();
	r_id_gen.reset();
	m_patch_needed=0;
	return *this;
	}

const Twins<size_t>& DependencyGraphDefault::idRangeGet() const
	{
	if(m_patch_needed)
		{const_cast<DependencyGraphDefault*>(this)->targetsPatch();}
	return m_id_range;
	}
