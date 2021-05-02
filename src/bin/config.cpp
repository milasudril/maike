//@	{
//@	  "targets":[{"name":"config.o","type":"object"}]
//@	}

#include "./config.hpp"

#include "src/io/input_file.hpp"

std::map<std::string, std::reference_wrapper<Maike::SourceFileInfoLoaders::Loader const>>
Maike::mapSourceFileInfoLoaders(std::reference_wrapper<Maike::Config const> cfg)
{
	auto& name_map = cfg.get().sourceTreeLoader().fileInfoLoaders();
	auto& loaders = cfg.get().sourceFileInfoLoaders().loaders();
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
	try
	{
		KeyValueStore::Compound cfg;
		std::for_each(std::begin(cfg_files), std::end(cfg_files), [&cfg](auto const& item) {
			try
			{
				Maike::Io::InputFile cfg_file{item};
				cfg |= KeyValueStore::Compound{Maike::Io::Reader{cfg_file}, item.string()};
			}
			catch(...)
			{
			}
		});
		return Config{cfg.get<Maike::KeyValueStore::CompoundRefConst>("maikeconfig")};
	}
	catch(...)
	{
	}
	return Config{};
}

Maike::Config Maike::resolveRecipes(Config const& cfg,
                                    SourceTreeLoader::CommandDictionary const& commands)
{
	Config ret;
	ret.dirCompiler(Db::Compiler{cfg.dirCompiler()}.resolveRecipe(commands));
	auto loaders = cfg.sourceFileInfoLoaders().loaders();
	std::for_each(std::begin(loaders), std::end(loaders), [&commands](auto& item) {
		item.second.compiler(Db::Compiler{item.second.compiler()}.resolveRecipe(commands));
	});
	SourceFileInfoLoaders::Config loaders_new;
	loaders_new.loaders(std::move(loaders));
	ret.sourceFileInfoLoaders(std::move(loaders_new));
	return ret;
}
