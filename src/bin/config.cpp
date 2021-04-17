//@	{
//@	  "targets":[{"name":"config.o","type":"object"}]
//@	}

#include "./config.hpp"

#include "src/io/input_file.hpp"

std::map<std::string, std::reference_wrapper<Maike::SourceFileInfoLoaders::Loader const>>
Maike::mapSourceFileInfoLoaders(Maike::Config const& cfg)
{
	auto& name_map = cfg.sourceTreeLoader().fileInfoLoaders();
	auto& loaders = cfg.sourceFileInfoLoaders().loaders();
	std::map<std::string, std::reference_wrapper<Maike::SourceFileInfoLoaders::Loader const>> ret;
	std::for_each(std::begin(name_map), std::end(name_map), [&ret, &loaders](auto const& item) {
		auto i = loaders.find(item.second);
		if(i == std::end(loaders))
		{
			throw std::runtime_error{std::string{"No loader of type "} + item.second
			                         + " has been configured"};
		}
		ret.insert_or_assign(std::end(ret), item.first, i->second);
	});
	return ret;
}

Maike::Config Maike::loadConfig(std::vector<fs::path> const& cfg_files)
{
	Config cfg;
	std::for_each(std::begin(cfg_files), std::end(cfg_files), [&cfg](auto const& item) {
		try
		{
			Maike::Io::InputFile cfg_file{item};
			auto cfg_json = KeyValueStore::Compound{Maike::Io::Reader{cfg_file}, item.string()};
			cfg = Config{cfg_json.get<Maike::KeyValueStore::CompoundRefConst>("maikeconfig")};
		}
		catch(...)
		{
		}
	});

	return cfg;
}