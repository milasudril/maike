//@	{
//@	"targets":[{"name":"spiderdefault.o","type":"object"}]
//@	}

#include "spiderdefault.hpp"
#include "directorylister.hpp"
#include "target_factorydelegator.hpp"
#include "stringkey.hpp"

using namespace Maike;

SpiderDefault::SpiderDefault(Target_FactoryDelegator& target_creator, DependencyGraph& targets):
   r_target_creator(target_creator), r_targets(targets)
{
}

SpiderDefault& SpiderDefault::scanFile(const char* filename, const char* in_dir)
{
	if(m_files_visited.find(VisitedKey(filename)) == m_files_visited.end())
	{
		m_files_to_scan.push({filename, in_dir});
		m_files_visited.insert(VisitedKey(filename));
	}
	return *this;
}

SpiderDefault& SpiderDefault::run()
{
	while(!m_files_to_scan.empty())
	{
		auto p = std::move(m_files_to_scan.top());
		m_files_to_scan.pop();
		auto filename = p.first.c_str();
		auto in_dir = p.second.c_str();
		r_target_creator.targetsLoad(filename, in_dir, *this, r_targets);
	}

	return *this;
}
