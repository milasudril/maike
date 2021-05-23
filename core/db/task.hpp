//@{"dependencies_extra":[{"ref":"./task.o","rel":"implementation"}]}

#ifndef MAIKE_DB_TASK_HPP
#define MAIKE_DB_TASK_HPP

#include "./dependency.hpp"
#include "./source_file_record.hpp"
#include "./dependency_graph.hpp"
#include "./invoker.hpp"
#include "./log_format.hpp"

#include "core/build/info.hpp"
#include "core/exec/command.hpp"
#include "core/sched/batch.hpp"

#include <chrono>
#include <optional>

namespace Maike::Db
{
	class Task
	{
	public:
		class ForceRecompilation
		{
		public:
			explicit ForceRecompilation(bool value = true): m_value{value}
			{
			}

			constexpr operator bool() const
			{
				return m_value;
			}

		private:
			bool m_value;
		};

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
		              LogFormat output_format);

		bool waitUntilAvailable(Sched::Batch const& batch);

		std::optional<Exec::Result> runIfNecessary(ForceRecompilation force, Invoker invoker);

		SourceFileRecordConst node() const
		{
			return m_node;
		}

		Timestamps timestamps() const
		{
			return Timestamps{m_t_created, m_t_prepared, m_t_ready, m_t_completed};
		}

		Exec::Command const& command() const
		{
			return m_cmd;
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