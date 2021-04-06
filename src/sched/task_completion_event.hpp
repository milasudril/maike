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
			std::lock_guard lock{m_mtx};
			m_value = TaskResult::Failure;
			m_cv.notify_all();
		}

		void taskSuceceded()
		{
			std::lock_guard lock{m_mtx};
			m_value = TaskResult::Success;
			m_cv.notify_all();
		}

	private:
		mutable std::mutex m_mtx;
		mutable std::condition_variable m_cv;
		TaskResult m_value;
	};

	template<class Event>
	class TaskCompletionEventGuard
	{
	public:
		TaskCompletionEventGuard(TaskCompletionEventGuard&&) = delete;

		explicit TaskCompletionEventGuard(Event& event): m_event{event}, m_triggered{false}
		{
		}

		void taskSuceceded()
		{
			m_event.get().taskSuceceded();
			m_triggered = true;
		}

		~TaskCompletionEventGuard()
		{
			if(!m_triggered) { m_event.get().taskFailed(); }
		}

	private:
		std::reference_wrapper<Event> m_event;
		bool m_triggered;
	};
}

#endif