//@	{
//@	  "targets":[{"name":"config.o","type":"object"}]
//@	}

#include "./config.hpp"

#include "src/source_file_info_loaders/cxx/source_file_loader.hpp"

Maike::SourceFileInfoLoaders::Config::Config()
{
	m_loaders.insert(std::make_pair(std::string{"cxx"},                         Maike::SourceFileInfoLoaders::Loader{Cxx::SourceFileLoader{}}));
}

Maike::SourceFileInfoLoaders::Config::Config(KeyValueStore::CompoundRefConst items)
{
	std::for_each(std::begin(items), std::end(items), [this](auto const& item) {
		auto cfg = item.second.template as<Maike::KeyValueStore::CompoundRefConst>();
		if(item.first == std::string_view{"cxx"})
		{ m_loaders.insert_or_assign(std::end(m_loaders), item.first, Loader{Cxx::SourceFileLoader(cfg)}); }
	});
}

Maike::KeyValueStore::JsonHandle Maike::SourceFileInfoLoaders::toJson(Config const& cfg)
{
	auto const& loaders = cfg.loaders();
	KeyValueStore::Compound ret;

	std::for_each(std::begin(loaders), std::end(loaders), [&ret](auto const& item)
	{
		ret.set(item.first.c_str(), item.second);
	});

	return ret.takeHandle();
}