//@	{
//@	  "targets":[{"name":"mem_monitor.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./mem_monitor.hpp"

#include <unistd.h>
#include <sys/wait.h>

#include <cstring>
#include <cassert>

void* volatile buffer; // Must be volatile to avoid optimizer to remove memset

namespace Testcases
{
	void maikeWatchMem()
	{
		int parent_to_child[2];
		pipe(parent_to_child);
		int child_to_parent[2];
		pipe(child_to_parent);

		auto pid = fork();
		assert(pid != -1);
		if(pid == 0)
		{
			close(parent_to_child[1]);
			close(child_to_parent[0]);
			size_t size_alloced{};
			(void)read(parent_to_child[0], &size_alloced, sizeof(size_alloced));
			close(parent_to_child[0]);
			buffer = malloc(size_alloced);
			memset(buffer, 0, size_alloced);
			(void)write(child_to_parent[1], &size_alloced, sizeof(size_alloced));
			close(child_to_parent[1]);
			std::this_thread::sleep_for(std::chrono::seconds{2});
		}
		else
		{
			close(child_to_parent[1]);
			close(parent_to_child[0]);
			Maike::MemMonitor monitor{std::chrono::milliseconds{10}};
			std::this_thread::sleep_for(std::chrono::seconds{1});
			auto const mem_avail = monitor.memAvail();
			auto const size_alloced = mem_avail / 3;
			(void)write(parent_to_child[1], &size_alloced, sizeof(size_alloced));
			close(parent_to_child[1]);
			auto const wait_for = mem_avail - size_alloced / 3;
			size_t dummy{};
			(void)read(child_to_parent[0], &dummy, sizeof(dummy));
			assert(dummy == size_alloced);
			auto t0 = std::chrono::steady_clock::now();
			monitor.waitForMem(wait_for);
			auto t = std::chrono::steady_clock::now();
			printf(">>> Got it after %.7f s\n", std::chrono::duration<double>{t - t0}.count());
			int status;
			waitpid(pid, &status, 0);
			close(child_to_parent[0]);
			assert(t - t0 >= std::chrono::milliseconds{1900});
		}
	}
}

int main()
{
	Testcases::maikeWatchMem();
}