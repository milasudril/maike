//@	{
//@	 "targets":[{"name":"signaling_counter.hpp","type":"include"}]
//@	}

#ifndef MAIKE_SCHED_SIGNALINGCOUNTER_HPP
#define MAIKE_SCHED_SIGNALINGCOUNTER_HPP

#include <condition_variable>
#include <mutex>

namespace Maike::Sched
{
	template<class T>
	class SignalingCounter
	{
	public:
		explicit SignalingCounter(T init_val): m_value{init_val}
		{
		}

		void wait(T value)
		{
			std::unique_lock lock{m_mtx};
			m_cv.wait(lock, [this, value]() { return m_value == value; });
		}

		SignalingCounter& operator++()
		{
			std::lock_guard lock{m_mtx};
			++m_value;
			m_cv.notify_one();
			return *this;
		}

		SignalingCounter& operator--()
		{
			std::lock_guard lock{m_mtx};
			--m_value;
			m_cv.notify_one();
			return *this;
		}

		bool operator==(T value) const
		{
			std::lock_guard lock{m_mtx};
			return m_value == value;
		}

		bool operator!=(T value) const
		{
			return !(*this == value);
		}

		void lock()
		{
			++(*this);
		}

		void unlock()
		{
			--(*this);
		}

	private:
		mutable std::mutex m_mtx;
		mutable std::condition_variable m_cv;
		T m_value;
	};
}

#endif