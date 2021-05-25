//@	{"dependencies_extra":[{"ref":"./task_list.o","rel":"implementation"}]}

#ifndef MAIKE_DB_TASKLIST_HPP
#define MAIKE_DB_TASKLIST_HPP

#include "./task.hpp"
#include "./dependency_graph.hpp"
#include "./invoker.hpp"
#include "./compilation_log.hpp"
#include "./target.hpp"

#include "core/build/info.hpp"
#include "core/sched/thread_pool.hpp"

#include <atomic>
#include <list>
#include <memory>

namespace Maike::Db
{
	class TaskList
	{
	public:
		explicit TaskList(DependencyGraph const& graph,
		                  Build::Info const& build_info,
		                  CompilationLog& compilation_log);

		explicit TaskList(DependencyGraph const& graph,
		                  Build::Info const& build_info,
		                  CompilationLog& compilation_log,
		                  Target const& target);

		void process(Sched::ThreadPool& workers,
		             Invoker invoker,
		             Task::ForceRecompilation force_recompilation);

		TaskResult status(SourceFileId id) const
		{
			return m_results[static_cast<size_t>(id.value())];
		}

		size_t size() const
		{
			return m_size;
		}

	private:
		std::unique_ptr<std::atomic<TaskResult>[]> m_results;
		std::list<Task> m_tasks;
		std::reference_wrapper<CompilationLog> m_compilation_log;
		size_t m_size;
	};
}

#endif