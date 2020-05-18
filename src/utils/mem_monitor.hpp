//@	{
//@	  "targets":[{"name":"mem_monitor.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"mem_monitor.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_UTILS_MEMMONITOR_HPP
#define MAIKE_UTILS_MEMMONITOR_HPP

#include <chrono>
#include <condition_variable>
#include <thread>

namespace Maike
{
	class MemMonitor
	{
	public:
		explicit MemMonitor(std::chrono::duration<double> refresh_interval);

		~MemMonitor();

		void waitForMem(size_t capacity)
		{
			std::unique_lock<std::mutex> lock{m_val_mutex};
			m_cv.wait(lock, [capacity, this]() { return m_mem_avail >= capacity; });
		}

		size_t memAvail() const
		{
			std::lock_guard lock{m_val_mutex};
			return m_mem_avail;
		}

	private:
		std::condition_variable m_cv;
		mutable std::mutex m_val_mutex;
		size_t m_mem_avail;

		std::condition_variable m_stop_cond;
		std::mutex m_stop_mtx;
		bool m_stop;

		std::thread m_worker;
		void run(std::chrono::duration<double> refresh_interval);
	};
}

#endif
