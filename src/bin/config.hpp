//@	{
//@	  "targets":[{"name":"config.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"./config.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_BIN_CONFIG_HPP
#define MAIKE_BIN_CONFIG_HPP

#include "src/source_tree_loader/config.hpp"
#include "src/source_file_info_loaders/config.hpp"
#include "src/source_tree_loader/command_dictionary.hpp"

namespace Maike
{
	class Config
	{
	public:
		Config(): m_dir_compiler{"make_directory.py"}
		{
		}

		explicit Config(KeyValueStore::CompoundRefConst obj):
		   m_source_tree_loader{obj.get<SourceTreeLoader::Config>("source_tree_loader")},
		   m_source_file_info_loaders{obj.get<SourceFileInfoLoaders::Config>("source_file_info_loaders")},
		   m_dir_compiler{obj.get<Db::Compiler>("dir_compiler")}
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

		Db::Compiler const& dirCompiler() const
		{
			return m_dir_compiler;
		}

		Config& dirCompiler(Db::Compiler&& val)
		{
			m_dir_compiler = std::move(val);
			return *this;
		}

	private:
		SourceTreeLoader::Config m_source_tree_loader;
		SourceFileInfoLoaders::Config m_source_file_info_loaders;
		Db::Compiler m_dir_compiler;
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
		   .set("dir_compiler", cfg.dirCompiler())
		   .takeHandle();
	}

	std::map<std::string, std::reference_wrapper<SourceFileInfoLoaders::Loader const>>
	mapSourceFileInfoLoaders(std::reference_wrapper<Maike::Config const> cfg);

	Config loadConfig(std::vector<fs::path> const& cfg_files);

	Config resolveRecipes(Config const& cfg, SourceTreeLoader::CommandDictionary const& commands);
}

#endif