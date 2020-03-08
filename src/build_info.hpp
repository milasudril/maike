//@	{
//@	 "targets":[{"name":"build_info.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"build_info.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_BUILDINFO_HPP
#define MAIKE_BUILDINFO_HPP

#include "./vcs_state.hpp"

#include <chrono>
#include <array>
#include <cstddef>

namespace Maike
{
	class BuildId
	{
	public:
		BuildId();

		explicit BuildId(std::array<std::byte, 32> const& bytes): m_bytes{bytes}
		{
		}

		auto const& bytes() const
		{
			return m_bytes;
		}

	private:
		std::array<std::byte, 32> m_bytes;
	};

	inline bool operator==(BuildId const& a, BuildId const& b)
	{ return a.bytes() == b.bytes();}

	inline bool operator!=(BuildId const& a, BuildId const& b)
	{ return !(a == b); }

	std::string toString(BuildId const& id);

	class BuildInfo
	{
	public:
		BuildInfo(): m_start_time{std::chrono::system_clock::now()}
		{
		}

		explicit BuildInfo(VcsState&& vcs_state):
		   m_start_time{std::chrono::system_clock::now()},
		   m_vcs_state{std::move(vcs_state)}
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
		std::chrono::time_point<std::chrono::system_clock> m_start_time;
		VcsState m_vcs_state;
		BuildId m_build_id;
	};
}
#endif
