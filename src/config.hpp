//@	{
//@	  "targets":[{"name":"config.hpp","type":"include"}]
//@	}

#ifndef MAIKE_CONFIG_HPP
#define MAIKE_CONFIG_HPP

#include "./env_store.hpp"
#include "./input_filter.hpp"
#include "src/vcs_invoker/config.hpp"

namespace Maike
{
	class Config
	{
	public:
		Config(): m_target_dir{"%MAIKE_TARGETS%"}
		{
		}

		explicit Config(KeyValueStore::CompoundRefConst obj):
		   m_filter{obj.get<InputFilter>("input_filter")},
		   m_target_dir{obj.get<char const*>("target_dir")},
		   m_env{obj.get<EnvStore>("env")},
		   m_vcs_config{obj.get<VcsInvoker::Config>("vcs_config")}
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

		InputFilter const& inputFilter() const
		{
			return m_filter;
		}

		Config& inputFilter(InputFilter&& val)
		{
			m_filter = std::move(val);
			return *this;
		}

		std::string const& targetDir() const
		{
			return m_target_dir;
		}

		Config& targetDir(std::string&& target_dir)
		{
			m_target_dir = std::move(target_dir);
			return *this;
		}

	private:
		InputFilter m_filter;
		std::string m_target_dir;
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
		   .set("vcs_config", cfg.vcsConfig())
		   .set("input_filter", cfg.inputFilter())
		   .set("target_dir", cfg.targetDir())
		   .takeHandle();
	}
}

#endif