//@	{
//@	 "targets":[{"name":"thread_pool.test","type":"application", "autorun":1}]
//@	}

#include "./thread_pool.hpp"

#include <cassert>

#include <vector>
#include <map>

namespace Testcases
{
	void maikeThreadpool()
	{
		Maike::ThreadPool thread_pool;

		Maike::ThreadPool::TaskResult<std::vector<int>> res_a;
		Maike::ThreadPool::TaskResult<std::map<char, int>> res_b;
		Maike::ThreadPool::TaskResult<std::string> res_c;

		std::vector<int> const a = {1, 2, 3};
		std::map<char, int> const b = {{'a', 1}, {'b', 2}};
		std::string const c{"Hello, World"};
		thread_pool.addTask([&a](){return a;}, res_a)
			.addTask([&b](){return b;}, res_b)
			.addTask([&c](){return c;}, res_c);

		assert(a == res_a.get());
		assert(b == res_b.get());
		assert(c == res_c.get());
	}
}

int main()
{
	Testcases::maikeThreadpool();
	return 0;
}
