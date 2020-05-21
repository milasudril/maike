//@	{
//@	  "targets":[{"name":"main.o","type":"object"}]
//@	}

#include "./main.hpp"

std::map<std::string, std::reference_wrapper<Maike::SourceFileInfoLoaders::Loader const>>
	Maike::Config::mapSourceFileInfoLoaders(Maike::Config::Main const& cfg)
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