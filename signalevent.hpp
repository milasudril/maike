#ifndef MAIKE_SIGNALEVENT_HPP
#define MAIKE_SIGNALEVENT_HPP

#include <condition_variable>
#include <mutex>

namespace Maike
{
	class SignalEvent
	{
	public:
		enum class State : int
		{
			Cleared,
			Signaled
		};

		explicit SignalEvent(State state): m_state{state}
		{
		}

		SignalEvent& signal()
		{
			std::lock_guard<std::mutex> lock{m_mtx};
			m_state = State::Signaled;
			m_cv.notify_all();
			return *this;
		}

		SignalEvent& reset()
		{
			std::lock_guard<std::mutex> lock{m_mtx};
			m_state = State::Cleared;
			return *this;
		}

		void wait() const
		{
			std::unique_lock<std::mutex> lock{m_mtx};
			m_cv.wait(lock, [this]() { return m_state == State::Signaled; });
		}

	private:
		mutable std::mutex m_mtx;
		mutable std::condition_variable m_cv;
		State m_state;
	};
}

#endif