//@	{"target":{"name":"task.o"}}

#include "./task.hpp"

Maike::Db::Task::Task(DependencyGraph const& g,
                      SourceFileRecordConst node,
                      Build::Info const& build_info,
                      LogFormat output_format):
   m_use_deps{getUseDepsRecursive(g, node)},
   m_cmd{makeBuildCommand(node, build_info, m_use_deps, output_format)},
   m_node{node}
{
}

std::optional<Maike::Exec::Result> Maike::Db::Task::runIfNecessary(ForceRecompilation force,
                                                                   Invoker invoker)
{
	m_t_started = Clock::now();
	if(force || m_node.sourceFileInfo().rebuildPolicy() == RebuildPolicy::Always
	   || !isUpToDate(m_node, m_use_deps))
	{
		auto result = invoker.execve(m_cmd);
		m_t_completed = Clock::now();
		return result;
	}
	m_t_completed = Clock::now();

	return std::optional<Maike::Exec::Result>{};
}

namespace
{
	Maike::Db::TaskResult status(void const* obj,
	                             Maike::Db::TaskResult (*f)(void const*, Maike::Db::SourceFileId),
	                             std::vector<Maike::Db::Dependency> const& deps)
	{
		if(std::size(deps) == 0) { return Maike::Db::TaskResult::Success; }

		auto i = std::begin(deps);
		while(i != std::end(deps))
		{
			auto value = f(obj, i->reference());
			if(value != Maike::Db::TaskResult::Success) { return value; }
			++i;
		}

		return Maike::Db::TaskResult::Success;
	}
}

Maike::Db::TaskResult Maike::Db::Task::status(void const* obj,
                                              TaskResult (*f)(void const*, SourceFileId))
{
	auto const status_a = ::status(obj, f, m_node.sourceFileInfo().buildDeps());
	if(status_a != Db::TaskResult::Success) { return status_a; }

	auto const status_b = ::status(obj, f, m_use_deps);
	if(status_b != Db::TaskResult::Success) { return status_b; }

	return Db::TaskResult::Success;
}