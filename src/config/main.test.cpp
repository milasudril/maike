//@	{
//@	  "targets":[{"name":"main.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./main.hpp"

#include <cassert>

namespace Testcases
{
	void maikeConfigMainInit()
	{
		Maike::Config::Main cfg;
		assert(cfg.env() == Maike::EnvStore{});
		assert(cfg.vcsConfig() == Maike::VcsInvoker::Config{});
		assert(cfg.sourceTreeLoaderCfg() == Maike::SourceTreeLoader::Config{});
	}
}

int main()
{
	Testcases::maikeConfigMainInit();
	return 0;
}