//@	{
//@	  "targets":[{"name":"envcache.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./envcache.hpp"

#include <cassert>
#include <algorithm>
#include <cstring>

namespace Testcases
{
	void maikeEnvCacheInit()
	{
		assert(Maike::env.find("PATH") != std::end(Maike::env));
		assert(!Maike::env.dirty());
	}

	void maikeEnvCacheInsert()
	{
		assert(Maike::env.find("PATH") != std::end(Maike::env));
		assert(!Maike::env.insert(std::pair{std::string{"PATH"}, "Blah"}).second);
		assert(!Maike::env.dirty());

		assert(Maike::env.find("another_var_1") == std::end(Maike::env));
		auto res = Maike::env.insert(std::pair{std::string{"another_var_1"}, "Blah"});
		assert(res.second);
		assert(res.first->second == "Blah");
		assert(Maike::env.dirty());

		assert(::getenv("another_var_1") != nullptr);
		assert(!Maike::env.dirty());
		assert(strcmp(::getenv("another_var_1"), "Blah") == 0);

		auto i = Maike::env.find("another_var_1");
		assert(i != Maike::env.end());
		assert(i->first == "another_var_1");
		assert(i->second == "Blah");
	}

	void maikeEnvCacheErase()
	{
		assert(Maike::env.find("PATH") != std::end(Maike::env));
		Maike::env.erase("PATH");
		assert(Maike::env.dirty());

		assert(::getenv("PATH") == nullptr);
		assert(!Maike::env.dirty());

		Maike::env.erase("PATH");
		assert(!Maike::env.dirty());
	}

	void maikeEnvSetenv()
	{
		assert(Maike::env.find("another_var_2") == std::end(Maike::env));
		::setenv("another_var_2", "foobar", 0);
		assert(Maike::env.dirty());
		assert(Maike::env.find("another_var_2") != std::end(Maike::env));
		assert(::getenv("another_var_2") != nullptr);
		assert(!Maike::env.dirty());

		::setenv("another_var_2", "foobar 2", 0);
		assert(!Maike::env.dirty());

		::setenv("another_var_2", "foobar 2", 1);
		assert(Maike::env.dirty());
		assert(::getenv("another_var_2") != nullptr);
		assert(!Maike::env.dirty());
	}

	void maikeEnvUnsetenv()
	{
		assert(Maike::env.find("PWD") != std::end(Maike::env));
		unsetenv("PWD");
		assert(Maike::env.dirty());

		assert(::getenv("PATH") == nullptr);
		assert(!Maike::env.dirty());
	}
}

int main()
{
	Testcases::maikeEnvCacheInit();
	Testcases::maikeEnvCacheInsert();
	Testcases::maikeEnvCacheErase();
	Testcases::maikeEnvSetenv();
	Testcases::maikeEnvUnsetenv();
}