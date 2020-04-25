//@	{
//@	  "targets":[{"name":"env.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./env.hpp"

#include <cassert>
#include <map>
#include <algorithm>
#include <cstring>
#include <string>

extern "C" char** environ;


namespace Testcases
{
	void maikeEnvLoad()
	{
		auto env = Maike::Env::load<std::map<std::string, std::string, std::less<>>>(environ);
		assert(env.find("PATH") != std::end(env));
	}

	void maikeEnvLoadStore()
	{
		auto env = Maike::Env::load<std::map<std::string, std::string, std::less<>>>(environ);
		Maike::Env::StringPointers test{env};

		auto env2 = Maike::Env::load<std::map<std::string, std::string, std::less<>>>(test.get());
		assert(env == env2);
	}
}

int main()
{
	Testcases::maikeEnvLoad();
	Testcases::maikeEnvLoadStore();
}