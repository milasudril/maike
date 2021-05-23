//@{"dependencies_extra":[{"ref":"./task.o","rel":"implementation"}]}

#ifndef MAIKE_DB_TASK_HPP
#define MAIKE_DB_TASK_HPP

#include "./dependency.hpp"
#include "./source_file_record.hpp"
#include "./dependency_graph.hpp"
#include "./compilation_log.hpp"
#include "./invoker.hpp"

#include "core/build/info.hpp"
#include "core/exec/command.hpp"
#include "core/sched/batch.hpp"

#include <chrono>

namespace Maike::Db
{
	class Task
	{
	public:
		using Clock = std::chrono::steady_clock;
		using TimePoint = Clock::time_point;

		struct Timestamps
		{
			TimePoint created;
			TimePoint prepared;
			TimePoint ready;
			TimePoint completed;
		};

		explicit Task(DependencyGraph const& g,
		              SourceFileRecordConst node,
		              Build::Info const& build_info,
		              CompilationLog::OutputFormat output_format);

		bool waitUntilAvailable(Sched::Batch const& batch);

		Exec::Result runIfNecessary(bool force, Invoker invoker);

		SourceFileRecordConst node() const
		{
			return m_node;
		}

		Timestamps timestamps() const
		{
			return Timestamps{m_t_created, m_t_prepared, m_t_ready, m_t_completed};
		}

	private:
		TimePoint m_t_created;
		std::vector<Dependency> m_use_deps;
		Exec::Command m_cmd;
		SourceFileRecordConst m_node;
		TimePoint m_t_prepared;
		TimePoint m_t_ready;
		TimePoint m_t_completed;
	};
}

#endif