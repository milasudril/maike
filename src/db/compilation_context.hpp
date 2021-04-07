//@	{
//@	 "targets":[{"name":"compilation_context.hpp","type":"include"}]
//@	}

#ifndef MAIKE_DB_COMPILATIONCONTEXT_HPP
#define MAINE_DB_COMPILATIONCONTEXT_HPP

#include "./source_file_id.hpp"

#include "src/sched/task_completion_event.hpp"
#include "src/sched/thread_pool.hpp"
#include "src/sched/task_counter.hpp"
#include "src/sched/signaling_counter.hpp"
#include "src/utils/scope_exit_handler.hpp"

namespace Maike::Db
{
	class NodeProcess
	{
	};

	using NodeProcessCounter = Sched::TaskCounter<NodeProcess>;

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
			if(auto e = m_threads.get().takeLastException(); e != nullptr) { std::rethrow_exception(e); }
			m_threads.get().addTask([&tasks_completed = m_tasks_completed,
			                         event = Sched::TaskCompletionEventGuard{m_events[id.value()]},
			                         func = std::forward<Callable>(f)]() mutable {
				ScopeExitHandler on_exit{[&tasks_completed]() { ++tasks_completed; }};
				func();
				event.taskSuceceded();
			});
			++m_tasks_sched;
		}

		NodeProcessCounter taskCount() const
		{
			return m_tasks_sched;
		}

		CompilationContext& throwAnyPendingException()
		{
			m_tasks_completed.wait(m_tasks_sched);
			if(auto e = m_threads.get().takeLastException(); e != nullptr) { std::rethrow_exception(e); }
			return *this;
		}

	private:
		std::unique_ptr<Sched::TaskCompletionEvent[]> m_events;
		std::reference_wrapper<Sched::ThreadPool> m_threads;
		NodeProcessCounter m_tasks_sched;
		Sched::SignalingCounter<NodeProcessCounter> m_tasks_completed;
	};
}

#endif