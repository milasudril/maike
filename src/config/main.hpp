//@	{
//@	  "targets":[{"name":"main.hpp","type":"include"}]
//@	}

#ifndef MAIKE_CONFIG_MAIN_HPP
#define MAIKE_CONFIG_MAIN_HPP

#include "src/env/env_store.hpp"
#include "src/vcs_invoker/config.hpp"
#include "src/source_tree_loader/config.hpp"

namespace Maike::Config
{
	class Main
	{
	public:
		Main()
		{
		}

		explicit Main(KeyValueStore::CompoundRefConst obj):
		   m_src_tree_loader_cfg{obj.get<SourceTreeLoader::Config>("source_tree_loader")},
		   m_env{obj.get<EnvStore>("env")}
		//		   m_vcs_config{obj.get<VcsInvoker::Main>("vcs_config")}
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

		VcsInvoker::Config const& vcsConfig() const
		{
			return m_vcs_config;
		}

		Main& vcsConfig(VcsInvoker::Config&& val)
		{
			m_vcs_config = std::move(val);
			return *this;
		}

		SourceTreeLoader::Config const& sourceTreeLoaderCfg() const
		{
			return m_src_tree_loader_cfg;
		}

		Main& sourceTreeLoaderCfg(SourceTreeLoader::Config&& val)
		{
			m_src_tree_loader_cfg = std::move(val);
			return *this;
		}

	private:
		SourceTreeLoader::Config m_src_tree_loader_cfg;
		EnvStore m_env;
		VcsInvoker::Config m_vcs_config;
	};

	inline auto fromJson(KeyValueStore::Empty<Main>, KeyValueStore::JsonRefConst ref)
	{
		return Main{ref.as<KeyValueStore::CompoundRefConst>()};
	}

	inline auto toJson(Main const& cfg)
	{
		return KeyValueStore::Compound{}
		   .set("env", cfg.env())
		   .set("vcs", cfg.vcsConfig())
		   .set("source_tree_loader", cfg.sourceTreeLoaderCfg())
		   .takeHandle();
	}
}

#endif