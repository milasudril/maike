//@	{"target":{"name":"task.o"}}

#include "./task.hpp"

#include <algorithm>

Maike::Db::Task::Task(DependencyGraph const& g,
                      SourceFileRecordConst node,
                      Build::Info const& build_info,
                      CompilationLog::OutputFormat output_format):
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

Maike::Exec::Result Maike::Db::Task::runIfNecessary(bool force, Invoker invoker)
{
	if(force || m_node.sourceFileInfo().rebuildPolicy() == RebuildPolicy::Always
	   || !isUpToDate(m_node, m_use_deps))
	{
		auto result = invoker.execve(m_cmd);
		m_t_completed = Clock::now();
		return result;
	}
	m_t_completed = Clock::now();

	return Exec::Result{Exec::ExitStatus{}};
}