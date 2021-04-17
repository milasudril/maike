//@	{
//@	 "targets":[{"name":"build_info.hpp","type":"include"}]
//@	}

#ifndef MAIKE_BIN_BUILDINFO_HPP
#define MAIKE_BIN_BUILDINFO_HPP

#include "./build_id.hpp"

#include "src/time/system_time_stamp.hpp"
#include "src/key_value_store/compound.hpp"

#include <chrono>
#include <array>
#include <cstddef>

namespace Maike
{
	class BuildInfo
	{
	public:
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
		BuildId m_build_id;
	};

	inline auto toJson(BuildInfo const& info)
	{
		return KeyValueStore::Compound{}
		   .set("start_time", std::chrono::system_clock::to_time_t(info.startTime().value()))
		   .set("build_id", info.buildId())
		   .takeHandle();
	}
}
#endif
