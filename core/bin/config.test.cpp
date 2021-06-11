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

		auto const& src_tree_loader_map = cfg.sourceTreeLoader().fileInfoLoaders();
		std::for_each(std::begin(src_tree_loader_map),
		              std::end(src_tree_loader_map),
		              [&loaders = cfg.sourceFileInfoLoaders().loaders()](auto const& item) {
			              assert(loaders.find(item.second) != std::end(loaders));
		              });
	}
}

int main()
{
	Testcases::maikeConfigMainInit();
	return 0;
}