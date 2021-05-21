//@	{
//@	  "targets":[{"name":"mem_monitor.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./mem_monitor.hpp"
#include "./callwrappers.hpp"
#include "./fs.hpp"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <cstring>
#include <cassert>

namespace
{
	void* volatile buffer; // Must be volatile to avoid optimizer to remove memset
}

namespace Testcases
{
	void maikeWatchMem()
	{
		auto pid = fork();
		assert(pid != -1);
		if(pid == 0)
		{
			Maike::fs::path executable{MAIKE_BUILDINFO_TARGETDIR};
			executable /= "core/utils/mem_monitor.test";
			execlp(executable.c_str(), executable.c_str(), "child", nullptr);
		}
		else
		{
			Maike::MemMonitor monitor{std::chrono::milliseconds{4}};
			std::this_thread::sleep_for(std::chrono::seconds{1});

			int sock = socket(AF_UNIX, SOCK_STREAM, 0);
			sockaddr_un addr;
			memset(&addr, 0, sizeof(addr));
			addr.sun_family = AF_UNIX;
			strcpy(addr.sun_path + 1, "child");
			connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

			auto const mem_avail = monitor.memAvail();
			auto const size_alloced = mem_avail / 3;
			Maike::unusedResult(write(sock, &size_alloced, sizeof(size_alloced)));
			auto const wait_for = mem_avail - size_alloced / 3;
			size_t dummy{};
			Maike::unusedResult(read(sock, &dummy, sizeof(dummy)));

			assert(dummy == size_alloced);
			auto t0 = std::chrono::steady_clock::now();
			monitor.waitForMem(wait_for);
			auto t = std::chrono::steady_clock::now();
			fprintf(stderr,
			        ">>> Got at least %zu bytes after %.7f s\n",
			        wait_for,
			        std::chrono::duration<double>{t - t0}.count());
			int status;
			waitpid(pid, &status, 0);
			close(sock);
			assert(t - t0 >= std::chrono::milliseconds{1900});
		}
	}
}

int main(int argc, char**)
{
	if(argc < 2) { Testcases::maikeWatchMem(); }
	else
	{
		int sock = socket(AF_UNIX, SOCK_STREAM, 0);
		sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path + 1, "child");
		bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
		listen(sock, 1);
		auto fd = accept(sock, nullptr, nullptr);
		size_t size_alloced{};
		Maike::unusedResult(read(fd, &size_alloced, sizeof(size_alloced)));
		buffer = malloc(size_alloced);
		memset(buffer, 0, size_alloced);
		Maike::unusedResult(write(fd, &size_alloced, sizeof(size_alloced)));
		std::this_thread::sleep_for(std::chrono::seconds{2});
	}
	return 0;
}
