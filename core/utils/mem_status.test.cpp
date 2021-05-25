//@	{
//@	  "targets":[{"name":"mem_status.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./mem_status.hpp"

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <thread>
#include <chrono>

namespace
{
	void* volatile buffer; // Must be volatile to avoid optimizer to remove memset
}

namespace Testcases
{
	void maikeGetMemAvail()
	{
		auto const n_before = Maike::getMemAvail();
		auto const n_malloc = n_before / 3;
		buffer = malloc(n_before / 3);
		memset(buffer, 'A', n_malloc);
		std::this_thread::sleep_for(std::chrono::duration<double>{1.0 / 1024});
		auto const n_after = Maike::getMemAvail();
		assert(n_after < n_before);
		free(buffer);
	}
}

int main()
{
	Testcases::maikeGetMemAvail();
	return 0;
}
