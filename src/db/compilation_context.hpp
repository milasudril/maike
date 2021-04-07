//@	{
//@	 "targets":[{"name":"compilation_context.hpp","type":"include"}]
//@	}

#ifndef MAIKE_DB_COMPILATIONCONTEXT_HPP
#define MAINE_DB_COMPILATIONCONTEXT_HPP

#include "./source_file_id.hpp"

#include "src/sched/task_completion_event.hpp"
#include "src/sched/thread_pool.hpp"
#include "src/sched/signaling_counter.hpp"
#include "src/utils/scope_exit_handler.hpp"

namespace Maike::Db
{
	class TaskCounter
	{
	public:
		constexpr TaskCounter(): m_n{0}
		{
		}

		constexpr size_t value() const
		{
			return m_n;
		}

		constexpr TaskCounter& operator++()
		{
			++m_n;
			return *this;
		}

		constexpr TaskCounter operator++(int)
		{
			auto ret = *this;
			++(*this);
			return ret;
		}

	private:
		size_t m_n;
	};

	constexpr bool operator==(TaskCounter a, TaskCounter b)
	{
		return a.value() == b.value();
	}

	constexpr bool operator!=(TaskCounter a, TaskCounter b)
	{
		return !(a == b);
	}

	class CompilationContext
	{
	public:
		explicit CompilationContext(size_t n_nodes, Sched::ThreadPool& threads):
		   m_events{std::make_unique<Sched::TaskCompletionEvent[]>(n_nodes)}, m_threads{threads}
		{
		}

		~CompilationContext()
		{
			m_tasks_completed.wait(m_tasks_sched);
		}

		bool taskFailed(SourceFileId id)
		{
			return m_events[id.value()].wait() == Sched::TaskResult::Failure;
		}

		template<class Callable>
		void process(SourceFileId id, Callable&& f)
		{
			m_threads.get().addTask([&tasks_completed = m_tasks_completed,
			                         event = Sched::TaskCompletionEventGuard{m_events[id.value()]},
			                         func = std::forward<Callable>(f)]() mutable {
				ScopeExitHandler on_exit{[&tasks_completed]() { ++tasks_completed; }};
				func();
				event.taskSuceceded();
			});
			++m_tasks_sched;
		}

		TaskCounter taskCount() const
		{
			return m_tasks_sched;
		}

	private:
		std::unique_ptr<Sched::TaskCompletionEvent[]> m_events;
		std::reference_wrapper<Sched::ThreadPool> m_threads;
		TaskCounter m_tasks_sched;
		Sched::SignalingCounter<TaskCounter> m_tasks_completed;
	};
}

#endif