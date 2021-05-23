#ifndef MAIKE_SCHED_BATCH_HPP
#define MAIKE_SCHED_BATCH_HPP

#include "core/sched/task_completion_event.hpp"
#include "core/sched/thread_pool.hpp"
#include "core/sched/task_counter.hpp"
#include "core/sched/signaling_counter.hpp"
#include "core/utils/scope_exit_handler.hpp"

namespace Maike::Sched
{
	class Batch
	{
	public:
		explicit Batch(size_t size, Sched::ThreadPool& threads):
		   m_events{std::make_unique<Sched::TaskCompletionEvent[]>(size)},
		   m_threads{threads},
		   m_tasks_sched{0},
		   m_tasks_completed{0}
		{
		}

		~Batch()
		{
			m_tasks_completed.wait(m_tasks_sched);
		}

		bool taskFailed(size_t task_index) const
		{
			return m_events[task_index].wait() == TaskResult::Failure;
		}

		TaskResult status(size_t task_index) const
		{
			return m_events[task_index].status();
		}

		template<class Callable>
		void add(size_t task_index, Callable&& f)
		{
			if(auto e = m_threads.get().takeLastException(); e != nullptr) { std::rethrow_exception(e); }
			m_threads.get().addTask([&tasks_completed = m_tasks_completed,
			                         event = Sched::TaskCompletionEventGuard{m_events[task_index]},
			                         func = std::forward<Callable>(f)]() mutable {
				ScopeExitHandler on_exit{[&tasks_completed]() { ++tasks_completed; }};
				func();
				event.taskSuceceded();
			});
			++m_tasks_sched;
		}

		size_t taskCount() const
		{
			return m_tasks_sched;
		}

		Batch& throwAnyPendingException()
		{
			m_tasks_completed.wait(m_tasks_sched);
			if(auto e = m_threads.get().takeLastException(); e != nullptr) { std::rethrow_exception(e); }
			return *this;
		}

	private:
		std::unique_ptr<Sched::TaskCompletionEvent[]> m_events;
		std::reference_wrapper<Sched::ThreadPool> m_threads;
		size_t m_tasks_sched;
		Sched::SignalingCounter<size_t> m_tasks_completed;
	};
}

#endif
