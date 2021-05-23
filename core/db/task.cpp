//@	{"target":{"name":"task.o"}}

#include "./task.hpp"

#include <algorithm>

Maike::Db::Task::Task(DependencyGraph const& g,
                      SourceFileRecordConst node,
                      Build::Info const& build_info,
                      LogFormat output_format):
   m_t_created{Clock::now()},
   m_use_deps{getUseDepsRecursive(g, node)},
   m_cmd{makeBuildCommand(node, build_info, m_use_deps, output_format)},
   m_node{node},
   m_t_prepared{Clock::now()}
{
}

bool Maike::Db::Task::waitUntilAvailable(Sched::Batch const& batch)
{
	auto const& build_deps = m_node.sourceFileInfo().buildDeps();
	if(std::any_of(std::begin(build_deps), std::end(build_deps), [&batch](auto const& item) {
		   return batch.taskFailed(item.reference().value());
	   }))
	{ return false; }

	// Wait until use deps for all build deps have been processed
	if(std::any_of(std::begin(m_use_deps), std::end(m_use_deps), [&batch](auto const& item) {
		   return batch.taskFailed(item.reference().value());
	   }))
	{ return false; }

	m_t_ready = Clock::now();
	return true;
}

std::optional<Maike::Exec::Result> Maike::Db::Task::runIfNecessary(ForceRecompilation force,
                                                                   Invoker invoker)
{
	m_t_ready = Clock::now();
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
	Maike::Sched::TaskResult status(std::vector<Maike::Db::Dependency> const& deps,
	                                Maike::Sched::Batch const& batch)
	{
		if(std::size(deps) == 0) { return Maike::Sched::TaskResult::Success; }

		auto i = std::begin(deps);
		while(i != std::end(deps))
		{
			auto value = batch.status(i->reference().value());
			if(value != Maike::Sched::TaskResult::Success) { return value; }
			++i;
		}

		return Maike::Sched::TaskResult::Success;
	}
}

Maike::Sched::TaskResult Maike::Db::Task::status(Sched::Batch const& batch)
{
	auto const status_a = ::status(m_node.sourceFileInfo().buildDeps(), batch);
	if(status_a != Sched::TaskResult::Success) { return status_a; }

	auto const status_b = ::status(m_use_deps, batch);
	if(status_b != Sched::TaskResult::Success) { return status_b; }

	return Sched::TaskResult::Success;
}