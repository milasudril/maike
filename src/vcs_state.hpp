//@	{"targets":[{"name":"vcs_state.hpp","type":"include"}]}

#ifndef MAIKE_VCSSTATE_HPP
#define MAIKE_VCSSTATE_HPP

#include <string>
#include <type_traits>

namespace Maike
{
	class VcsState
	{
	public:
		template<class Source, std::enable_if_t<!std::is_same_v<Source, VcsState>>>
		explicit VcsState(Source const& src):
		 m_revision{src.revision()}
		,m_version_tag{src.versionTag()}
		,m_branch{src.branch()}
		{
		}
	
		std::string const& revision() const
		{
			return *m_revision;
		}
		
		std::string const& versionTag() const
		{
			return *m_version_tag;
		}
		
		std::stirng const& branch() const
		{
			return *m_branch;
		}

	private:
		std::string m_revision;
		std::string m_version_tag;
		std::string m_branch;
	};
}
#endif
