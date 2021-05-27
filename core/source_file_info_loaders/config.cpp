//@	{
//@	  "targets":[{"name":"config.o","type":"object"}]
//@	}

#include "./config.hpp"

#include "core/source_file_info_loaders/cxx/source_file_loader.hpp"
#include "core/source_file_info_loaders/app/source_file_loader.hpp"
#include "core/source_file_info_loaders/extension/source_file_loader.hpp"
#include "core/source_file_info_loaders/generic/source_file_loader.hpp"

#include <algorithm>

Maike::SourceFileInfoLoaders::Config::Config()
{
	m_loaders.insert(
	   std::pair{std::string{"cxx"}, SourceFileInfoLoaders::Loader{Cxx::SourceFileLoader{}}});
	m_loaders.insert(
	   std::pair{std::string{"app"}, SourceFileInfoLoaders::Loader{App::SourceFileLoader{}}});
	m_loaders.insert(
	   std::pair{std::string{"lib"}, SourceFileInfoLoaders::Loader{App::SourceFileLoader{}}});
	m_loaders.insert(std::pair{std::string{"extension"},
	                           SourceFileInfoLoaders::Loader{Extension::SourceFileLoader{}}});
	m_loaders.insert(std::pair{std::string{"generic_example"},
	                           SourceFileInfoLoaders::Loader{Generic::SourceFileLoader{}}});
}

namespace
{
	template<class T>
	T makeInstance(Maike::KeyValueStore::CompoundRefConst item)
	{
		auto const config = item.template getIf<T>("config");
		return config ? *config : T{};
	}
}

Maike::SourceFileInfoLoaders::Config::Config(KeyValueStore::CompoundRefConst items)
{
	std::for_each(std::begin(items), std::end(items), [this](auto const& item) {
		auto const cfg = item.second.template as<KeyValueStore::CompoundRefConst>();
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
		else if(loader == std::string_view{"generic"})
		{
			m_loaders.insert_or_assign(
			   std::end(m_loaders),
			   item.first,
			   Loader{makeInstance<Generic::SourceFileLoader>(cfg), std::move(compiler)});
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