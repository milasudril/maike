//@	{
//@	  "targets":[{"name":"config.hpp","type":"include"}]
//@	}

#ifndef MAIKE_CONFIG_HPP
#define MAIKE_CONFIG_HPP

#include "./env_store.hpp"
#include "src/vcs_invoker/config.hpp"
#include "src/source_tree_loader/config.hpp"

namespace Maike
{
	class Config
	{
	public:
		Config()
		{
		}

		explicit Config(KeyValueStore::CompoundRefConst obj):
		   m_src_tree_loader_cfg{obj.get<SourceTreeLoader::Config>("source_tree_loader")},
		   m_env{obj.get<EnvStore>("env")}
		//		   m_vcs_config{obj.get<VcsInvoker::Config>("vcs_config")}
		{
		}

		EnvStore const& env() const
		{
			return m_env;
		}

		Config& env(EnvStore&& val)
		{
			m_env = std::move(val);
			return *this;
		}

		VcsInvoker::Config const& vcsConfig() const
		{
			return m_vcs_config;
		}

		Config& vcsConfig(VcsInvoker::Config&& val)
		{
			m_vcs_config = std::move(val);
			return *this;
		}

		SourceTreeLoader::Config const& sourceTreeLoaderCfg() const
		{
			return m_src_tree_loader_cfg;
		}

		Config& sourceTreeLoaderCfg(SourceTreeLoader::Config&& val)
		{
			m_src_tree_loader_cfg = std::move(val);
			return *this;
		}

	private:
		SourceTreeLoader::Config m_src_tree_loader_cfg;
		EnvStore m_env;
		VcsInvoker::Config m_vcs_config;
	};

	inline auto fromJson(KeyValueStore::Empty<Config>, KeyValueStore::JsonRefConst ref)
	{
		return Config{ref.as<KeyValueStore::CompoundRefConst>()};
	}

	inline auto toJson(Config const& cfg)
	{
		return KeyValueStore::Compound{}
		   .set("env", cfg.env())
		   .set("vcs", cfg.vcsConfig())
		   .set("source_tree_loader", cfg.sourceTreeLoaderCfg())
		   .takeHandle();
	}
}

#endif