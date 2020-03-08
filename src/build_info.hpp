//@	{"targets":[{"name":"build_info.hpp","type":"include"}]}

#ifndef MAIKE_VCSSTATE_HPP
#define MAIKE_VCSSTATE_HPP

#include "./vcs_state.hpp"

#include <chrono>

namespace Maike
{
	class BuildInfo
	{
	public:
		BuildInfo():m_start_time{std::chrono::system_clock::now()}
		{}

		explicit BuildInfo(VcsState&& vcs_state):
		m_start_time{std::chrono::system_clock::now()}
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

	private:
		std::chrono::time_point<std::chrono::system_clock> m_start_time;
		VcsState m_vcs_state;
	};
}
#endif
