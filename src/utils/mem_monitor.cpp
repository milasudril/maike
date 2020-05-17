//@	{
//@	  "targets":
//@		[{"name":"mem_monitor.o","type":"object",
//@		"dependencies":[{"ref":"pthread", "rel":"external"}]}]
//@	}

#include "./mem_monitor.hpp"

#include <unistd.h>
#include <fcntl.h>

#include <cstdlib>

Maike::MemMonitor::MemMonitor(std::chrono::duration<double> refresh_interval):
   m_mem_avail{0},
   m_stop{false}
{
	m_worker = std::thread(&Maike::MemMonitor::run, this, refresh_interval);
}

Maike::MemMonitor::~MemMonitor()
{
	{
		std::lock_guard lock{m_stop_mtx};
		m_stop = true;
		m_stop_cond.notify_one();
	}
	m_worker.join();
}

namespace
{
	size_t get_mem_avail(char* buffer, char* buffer_end)
	{
		enum class State : int
		{
			Key,
			Value,
			Skip
		};
		auto state_current = State::Key;
		auto pos_0 = buffer;
		while(buffer != buffer_end)
		{
			auto ch_in = *buffer;
			++buffer;
			switch(state_current)
			{
				case State::Key:
					switch(ch_in)
					{
						case ':':
							if(std::string_view{pos_0, static_cast<size_t>(buffer - pos_0)} == "MemAvailable:")
							{
								state_current = State::Value;
								pos_0 = buffer;
							}
							else
							{
								state_current = State::Skip;
							}

							break;
					}
					break;

				case State::Value:
					switch(ch_in)
					{
						case '\n':
							*(buffer - 1) = '\0';                            // To be able to use atoll
							return 1024 * static_cast<size_t>(atoll(pos_0)); // Reports are in kB
					}
					break;

				case State::Skip:
					switch(ch_in)
					{
						case '\n': state_current = State::Key; pos_0 = buffer;
					}
					break;
			}
		}
		return 0;
	}
}


void Maike::MemMonitor::run(std::chrono::duration<double> refresh_interval)
{
	auto wait = [this](auto duration) {
		std::unique_lock lock{m_stop_mtx};
		return m_stop_cond.wait_for(lock, duration, [this]() { return m_stop; });
	};

	int const fd = open("/proc/meminfo", O_RDONLY);
	std::array<char, 4096> buffer{};
	do
	{
		auto n = pread(fd, std::begin(buffer), std::size(buffer), 0);
		auto mem_avail = get_mem_avail(std::begin(buffer), std::begin(buffer) + n);
		std::lock_guard lock{m_val_mutex};
		m_mem_avail = mem_avail;
		m_cv.notify_all();
	} while(!wait(refresh_interval));
	close(fd);
}