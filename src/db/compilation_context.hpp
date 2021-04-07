//@	{
//@	 "targets":[{"name":"compilation_context.hpp","type":"include"}]
//@	}

#ifndef MAIKE_DB_COMPILATIONCONTEXT_HPP
#define MAINE_DB_COMPILATIONCONTEXT_HPP

#include "./source_file_id.hpp"

#include "src/sched/task_completion_event.hpp"
#include "src/sched/thread_pool.hpp"

#include <thread>

namespace Maike::Db
{
	class CompilationContext
	{
	public:
		explicit CompilationContext(size_t n_targets, Sched::ThreadPool& workers):
		   m_events{std::make_unique<Sched::TaskCompletionEvent[]>(n_targets)}, m_workers{workers}
		{
		}

		Sched::TaskResult waitForTarget(SourceFileId id)
		{
			return m_events[id.value()].wait();
		}

		Sched::TaskCompletionEventGuard<Sched::TaskCompletionEvent> taskCompletionEvent(SourceFileId id)
		{
			return Sched::TaskCompletionEventGuard{m_events[id.value()]};
		}


	private:
		std::unique_ptr<Sched::TaskCompletionEvent[]> m_events;
		std::reference_wrapper<Sched::ThreadPool> m_workers;
	};
}

#endif