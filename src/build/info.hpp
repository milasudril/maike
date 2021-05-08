//@	{
//@	}

#ifndef MAIKE_BUILD_INFO_HPP
#define MAIKE_BUILD_INFO_HPP

#include "./id.hpp"

#include "src/utils/system_time_stamp.hpp"
#include "src/key_value_store/compound.hpp"

#include <chrono>
#include <array>
#include <cstddef>

namespace Maike::Build
{
	class Info
	{
	public:
		Info(): m_source_dir{"."}, m_target_dir{"__targets"}
		{
		}

		Info& startTime(SystemTimeStamp val)
		{
			m_start_time = val;
			return *this;
		}

		SystemTimeStamp startTime() const
		{
			return m_start_time;
		}

		Info& buildId(Id&& id)
		{
			m_build_id = std::move(id);
			return *this;
		}

		Id const& buildId() const
		{
			return m_build_id;
		}

		Info& sourceDir(fs::path&& val)
		{
			m_source_dir = std::move(val);
			return *this;
		}

		fs::path const& sourceDir() const
		{
			return m_source_dir;
		}

		Info& targetDir(fs::path&& val)
		{
			m_target_dir = std::move(val);
			return *this;
		}

		fs::path const& targetDir() const
		{
			return m_target_dir;
		}

	private:
		SystemTimeStamp m_start_time;
		Id m_build_id;
		fs::path m_source_dir;
		fs::path m_target_dir;
	};

	inline auto toJson(Info const& info)
	{
		return KeyValueStore::Compound{}
		   .set("start_time", std::chrono::system_clock::to_time_t(info.startTime().value()))
		   .set("build_id", info.buildId())
		   .set("source_dir", info.sourceDir())
		   .set("target_dir", info.targetDir())
		   .takeHandle();
	}
}
#endif
