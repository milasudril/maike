//@	{
//@	  "targets":[{"name":"config.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./config.hpp"

#include <cassert>

namespace Testcases
{
	void maikeConfigInit()
	{
		Maike::Config cfg;
		assert(cfg.env() == Maike::EnvStore{});
		assert(cfg.vcsConfig() == Maike::VcsInvoker::Config{});
		assert(cfg.sourceTreeLoaderCfg() == Maike::SourceTreeLoader::Config{});
	}
}

int main()
{
	Testcases::maikeConfigInit();
	return 0;
}