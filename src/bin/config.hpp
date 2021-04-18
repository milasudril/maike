//@	{
//@	  "targets":[{"name":"config.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"config.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_BIN_CONFIG_HPP
#define MAIKE_BIN_CONFIG_HPP

#include "src/env/env_store.hpp"
#include "src/source_tree_loader/config.hpp"
#include "src/source_file_info_loaders/config.hpp"

namespace Maike
{
	class Config
	{
	public:
		Config() = default;

		explicit Config(KeyValueStore::CompoundRefConst obj):
		   m_source_tree_loader{obj.get<SourceTreeLoader::Config>("source_tree_loader")},
		   m_source_file_info_loaders{obj.get<SourceFileInfoLoaders::Config>("source_file_info_loaders")}
		{
		}

		SourceTreeLoader::Config const& sourceTreeLoader() const
		{
			return m_source_tree_loader;
		}

		Config& sourceTreeLoader(SourceTreeLoader::Config&& val)
		{
			m_source_tree_loader = std::move(val);
			return *this;
		}

		SourceFileInfoLoaders::Config const& sourceFileInfoLoaders() const
		{
			return m_source_file_info_loaders;
		}

		Config& sourceFileInfoLoaders(SourceFileInfoLoaders::Config&& val)
		{
			m_source_file_info_loaders = std::move(val);
			return *this;
		}

	private:
		SourceTreeLoader::Config m_source_tree_loader;
		SourceFileInfoLoaders::Config m_source_file_info_loaders;
	};

	inline auto fromJson(KeyValueStore::Empty<Config>, KeyValueStore::JsonRefConst ref)
	{
		return Config{ref.as<KeyValueStore::CompoundRefConst>()};
	}

	inline auto toJson(Config const& cfg)
	{
		return KeyValueStore::Compound{}
		   .set("source_tree_loader", cfg.sourceTreeLoader())
		   .set("source_file_info_loaders", cfg.sourceFileInfoLoaders())
		   .takeHandle();
	}

	std::map<std::string, std::reference_wrapper<SourceFileInfoLoaders::Loader const>>
	mapSourceFileInfoLoaders(std::reference_wrapper<Maike::Config const> cfg);

	Config loadConfig(std::vector<fs::path> const& cfg_files);
}

#endif