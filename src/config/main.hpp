//@	{
//@	  "targets":[{"name":"main.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"main.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_CONFIG_MAIN_HPP
#define MAIKE_CONFIG_MAIN_HPP

#include "./utils.hpp"

#include "src/env/env_store.hpp"
#include "src/source_tree_loader/config.hpp"
#include "src/source_file_info_loaders/config.hpp"

namespace Maike::Config
{
	class Main
	{
	public:
		Main()
		{
		}

		explicit Main(KeyValueStore::CompoundRefConst obj):
		   m_source_tree_loader{obj.get<SourceTreeLoader::Config>("source_tree_loader")},
		   m_source_file_info_loaders{obj.get<SourceFileInfoLoaders::Config>("source_file_info_loaders")},
		   m_env{obj.get<EnvStore>("env")},
		   m_utils{obj.get<Utils>("utils")}
		{
		}

		EnvStore const& env() const
		{
			return m_env;
		}

		Main& env(EnvStore&& val)
		{
			m_env = std::move(val);
			return *this;
		}

		Utils const& utils() const
		{
			return m_utils;
		}

		Main& utils(Utils&& val)
		{
			m_utils = std::move(val);
			return *this;
		}

		SourceTreeLoader::Config const& sourceTreeLoader() const
		{
			return m_source_tree_loader;
		}

		Main& sourceTreeLoader(SourceTreeLoader::Config&& val)
		{
			m_source_tree_loader = std::move(val);
			return *this;
		}

		SourceFileInfoLoaders::Config const& sourceFileInfoLoaders() const
		{
			return m_source_file_info_loaders;
		}

		Main& sourceFileInfoLoaders(SourceFileInfoLoaders::Config&& val)
		{
			m_source_file_info_loaders = std::move(val);
			return *this;
		}

	private:
		SourceTreeLoader::Config m_source_tree_loader;
		SourceFileInfoLoaders::Config m_source_file_info_loaders;
		EnvStore m_env;
		Utils m_utils;
	};

	inline auto fromJson(KeyValueStore::Empty<Main>, KeyValueStore::JsonRefConst ref)
	{
		return Main{ref.as<KeyValueStore::CompoundRefConst>()};
	}

	inline auto toJson(Main const& cfg)
	{
		return KeyValueStore::Compound{}
		   .set("env", cfg.env())
		   .set("utils", cfg.utils())
		   .set("source_tree_loader", cfg.sourceTreeLoader())
		   .set("source_file_info_loaders", cfg.sourceFileInfoLoaders())
		   .takeHandle();
	}

	std::map<std::string, std::reference_wrapper<SourceFileInfoLoaders::Loader const>>
	mapSourceFileInfoLoaders(Main const& cfg);

	Main load(std::vector<fs::path> const& cfg_files);
}

#endif