//@	{"targets":[{"name":"vcs_state.hpp","type":"include"}]}

#ifndef MAIKE_VCSSTATE_HPP
#define MAIKE_VCSSTATE_HPP

namespace Maike
{
	class VcsState
	{
	public:

	private:
		std::string m_revision;
		std::string m_version_tag;
		std::string m_branch;
	};
}
#endif
