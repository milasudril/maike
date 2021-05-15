//@	{
//@	  "targets":[{"name":"config.o","type":"object"}]
//@	}

#include "./config.hpp"

#include "core/source_file_info_loaders/cxx/source_file_loader.hpp"
#include "core/source_file_info_loaders/app/source_file_loader.hpp"
#include "core/source_file_info_loaders/extension/source_file_loader.hpp"

#include <algorithm>

Maike::SourceFileInfoLoaders::Config::Config()
{
	m_loaders.insert(std::make_pair(std::string{"cxx"},
	                                Maike::SourceFileInfoLoaders::Loader{Cxx::SourceFileLoader{}}));
	m_loaders.insert(std::make_pair(std::string{"app"},
	                                Maike::SourceFileInfoLoaders::Loader{App::SourceFileLoader{}}));
	m_loaders.insert(std::make_pair(
	   std::string{"extension"}, Maike::SourceFileInfoLoaders::Loader{Extension::SourceFileLoader{}}));
}

Maike::SourceFileInfoLoaders::Config::Config(KeyValueStore::CompoundRefConst items)
{
	std::for_each(std::begin(items), std::end(items), [this](auto const& item) {
		auto const cfg = item.second.template as<Maike::KeyValueStore::CompoundRefConst>();
		auto const loader = cfg.template get<char const*>("loader");
		auto compiler = cfg.template get<Db::Compiler>("compiler");

		if(loader == std::string_view{"maikerule"})
		{
			m_loaders.insert_or_assign(
			   std::end(m_loaders), item.first, Loader{App::SourceFileLoader{}, std::move(compiler)});
		}
		else if(loader == std::string_view{"cxx_src_loader"})
		{
			m_loaders.insert_or_assign(
			   std::end(m_loaders), item.first, Loader{Cxx::SourceFileLoader{}, std::move(compiler)});
		}
		else if(loader == std::string_view{"extension"})
		{
			m_loaders.insert_or_assign(
			   std::end(m_loaders), item.first, Loader{Extension::SourceFileLoader{}, std::move(compiler)});
		}
		else
		{
			throw std::runtime_error{std::string{"Unsupported loader "} + loader};
		}
	});
}

Maike::KeyValueStore::JsonHandle Maike::SourceFileInfoLoaders::toJson(Config const& cfg)
{
	auto const& loaders = cfg.loaders();
	KeyValueStore::Compound ret;

	std::for_each(std::begin(loaders), std::end(loaders), [&ret](auto const& item) {
		ret.set(item.first.c_str(), item.second);
	});

	return ret.takeHandle();
}