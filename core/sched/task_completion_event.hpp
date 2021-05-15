//@	{
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
		std::mutex m_mtx;
		std::condition_variable m_cv;
		TaskResult m_value;
	};

	template<class Event>
	class TaskCompletionEventGuard
	{
	public:
		TaskCompletionEventGuard(TaskCompletionEventGuard&& other) noexcept:
		   m_event{other.m_event}, m_triggered{other.m_triggered}
		{
			other.m_triggered = true;
		}

		TaskCompletionEventGuard& operator=(TaskCompletionEventGuard&& other) noexcept
		{
			expire();
			m_event = other.m_event;
			m_triggered = other.m_triggered;
			other.m_triggered = true;
			return *this;
		}

		explicit TaskCompletionEventGuard(Event& event): m_event{event}, m_triggered{false}
		{
		}

		void expire()
		{
			if(!m_triggered)
			{
				m_event.get().taskFailed();
				m_triggered = true;
			}
		}

		void taskSuceceded()
		{
			m_event.get().taskSuceceded();
			m_triggered = true;
		}

		~TaskCompletionEventGuard()
		{
			expire();
		}

	private:
		std::reference_wrapper<Event> m_event;
		bool m_triggered;
	};
}

#endif
