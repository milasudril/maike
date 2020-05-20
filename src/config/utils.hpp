//@	{
//@	  "targets":[{"name":"utils.hpp","type":"include"}]
//@	}

#ifndef MAIKE_CONFIG_UTILS_HPP
#define MAIKE_CONFIG_UTILS_HPP

#include "src/vcs_invoker/config.hpp"

namespace Maike::Config
{
	class Utils
	{
	public:
		Utils()
		{
		}

		explicit Utils(KeyValueStore::CompoundRefConst obj):
		   m_vcs_config{obj.get<VcsInvoker::Config>("vcs")}
		{
		}

		VcsInvoker::Config const& vcsConfig() const
		{
			return m_vcs_config;
		}

		Utils& vcsConfig(VcsInvoker::Config&& val)
		{
			m_vcs_config = std::move(val);
			return *this;
		}

	private:
		VcsInvoker::Config m_vcs_config;
	};

	inline auto fromJson(KeyValueStore::Empty<Utils>, KeyValueStore::JsonRefConst ref)
	{
		return Utils{ref.as<KeyValueStore::CompoundRefConst>()};
	}

	inline auto toJson(Utils const& cfg)
	{
		return KeyValueStore::Compound{}.set("vcs", cfg.vcsConfig()).takeHandle();
	}
}

#endif