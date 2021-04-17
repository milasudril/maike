//@	{
//@	 "targets":[{"name":"info.hpp","type":"include"}]
//@	}

#ifndef MAIKE_BUILD_INFO_HPP
#define MAIKE_BUILD_INFO_HPP

#include "./id.hpp"

#include "src/time/system_time_stamp.hpp"
#include "src/key_value_store/compound.hpp"

#include <chrono>
#include <array>
#include <cstddef>

namespace Maike::Build
{
	class Info
	{
	public:
		auto startTime() const
		{
			return m_start_time;
		}

		Id const& buildId() const
		{
			return m_build_id;
		}

	private:
		SystemTimeStamp m_start_time;
		Id m_build_id;
	};

	inline auto toJson(Info const& info)
	{
		return KeyValueStore::Compound{}
		   .set("start_time", std::chrono::system_clock::to_time_t(info.startTime().value()))
		   .set("build_id", info.buildId())
		   .takeHandle();
	}
}
#endif
