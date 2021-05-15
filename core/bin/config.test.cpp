//@	{
//@	  "targets":[{"name":"config.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./config.hpp"

#include <cassert>

namespace Testcases
{
	void maikeConfigMainInit()
	{
		Maike::Config cfg;
		assert(cfg.sourceTreeLoader() == Maike::SourceTreeLoader::Config{});
		assert(cfg.sourceFileInfoLoaders() == Maike::SourceFileInfoLoaders::Config{});
	}
}

int main()
{
	Testcases::maikeConfigMainInit();
	return 0;
}