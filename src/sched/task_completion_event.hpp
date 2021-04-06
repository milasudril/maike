//@	{
//@	 "targets":[{"name":"task_completion_event.hpp","type":"include"}]
//@	}

#ifndef MAIKE_SCHED_TASKCOMPLETIONEVENT_HPP
#define MAIKE_SCHED_TASKCOMPLETIONEVENT_HPP

#include <condition_variable>
#include <mutex>

namespace Maike::Sched
{
	enum class TaskResult : int
	{
		Pending,
		Success,
		Failure
	};

	class TaskCompletionEvent
	{
	public:
		TaskCompletionEvent(): m_value{TaskResult::Pending}
		{
		}

		TaskResult wait()
		{
			std::unique_lock lock{m_mtx};
			m_cv.wait(lock, [this]() { return m_value != TaskResult::Pending; });
			return m_value;
		}

		void taskFailed()
		{
			m_value = TaskResult::Failure;
			m_cv.notify_all();
		}

		void taskSuceceded()
		{
			m_value = TaskResult::Success;
			m_cv.notify_all();
		}

	private:
		mutable std::mutex m_mtx;
		mutable std::condition_variable m_cv;
		TaskResult m_value;
	};
}

#endif