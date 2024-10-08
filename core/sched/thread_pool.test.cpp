//@	{
//@	 "targets":[{"name":"thread_pool.test","type":"application", "autorun":1}]
//@	}

#include "./thread_pool.hpp"

#include <cassert>

#include <vector>
#include <map>
#include <stdexcept>

namespace Testcases
{
	void maikeThreadpoolRunTasks()
	{
		Maike::Sched::ThreadPool thread_pool;

		Maike::Sched::ThreadPool::TaskResult<std::vector<int>> res_a;
		Maike::Sched::ThreadPool::TaskResult<std::map<char, int>> res_b;
		Maike::Sched::ThreadPool::TaskResult<std::string> res_c;

		std::vector<int> const a = {1, 2, 3};
		std::map<char, int> const b = {{'a', 1}, {'b', 2}};
		std::string const c{"Hello, World"};
		thread_pool.addTask([&a]() { return a; }, res_a)
		   .addTask([&b]() { return b; }, res_b)
		   .addTask([&c]() { return c; }, res_c);

		assert(a == res_a.get());
		assert(b == res_b.get());
		assert(c == res_c.get());

		auto tmp = res_a.take();
		assert(a == tmp);
	}

	void maikeThreadpoolTaskWithException()
	{
		Maike::Sched::ThreadPool thread_pool;
		Maike::Sched::ThreadPool::TaskResult<int> res;
		try
		{
			thread_pool.addTask([]() -> int {
				throw std::runtime_error{"Blah"};
				return -1;
			}, res);
			(void)res.take();
			abort();
		}
		catch(...)
		{
		}
	}
}

int main()
{
	Testcases::maikeThreadpoolRunTasks();
	Testcases::maikeThreadpoolTaskWithException();
	return 0;
}
