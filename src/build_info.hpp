//@	{
//@	 "targets":[{"name":"build_info.hpp","type":"include"}]
//@	}

#ifndef MAIKE_BUILDINFO_HPP
#define MAIKE_BUILDINFO_HPP

#include "./build_id.hpp"
#include "./system_time_stamp.hpp"
#include "./vcs_state.hpp"

#include "key_value_store/compound.hpp"

#include <chrono>
#include <array>
#include <cstddef>

namespace Maike
{
	class BuildInfo
	{
	public:
		explicit BuildInfo(VcsState&& vcs_state): m_vcs_state{std::move(vcs_state)}
		{
		}

		VcsState const& vcsState() const
		{
			return m_vcs_state;
		}

		auto startTime() const
		{
			return m_start_time;
		}

		BuildId const& buildId() const
		{
			return m_build_id;
		}

	private:
		SystemTimeStamp m_start_time;
		VcsState m_vcs_state;
		BuildId m_build_id;
	};

	inline auto toJson(BuildInfo const& info)
	{
		return KeyValueStore::Compound{}
		   .set("start_time", std::chrono::system_clock::to_time_t(info.startTime().value()))
		   .set("vcs_state", info.vcsState())
		   .set("build_id", info.buildId())
		   .takeHandle();
	}
}
#endif
