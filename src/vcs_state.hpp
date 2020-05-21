//@	{"targets":[{"name":"vcs_state.hpp","type":"include"}]}

#ifndef MAIKE_VCSSTATE_HPP
#define MAIKE_VCSSTATE_HPP

#include "src/key_value_store/compound.hpp"

#include <string>
#include <type_traits>

namespace Maike
{
	class VcsState
	{
	public:
		VcsState() = default;

		template<class Source>
		explicit VcsState(Source const& src,
		                  std::enable_if_t<!std::is_same_v<Source, VcsState>, int> = 0):
		   m_revision{src.revision()},
		   m_version_tag{src.versionTag()},
		   m_branch{src.branch()}
		{
		}

		std::string const& revision() const
		{
			return m_revision;
		}

		std::string const& versionTag() const
		{
			return m_version_tag;
		}

		std::string const& branch() const
		{
			return m_branch;
		}

	private:
		std::string m_revision;
		std::string m_version_tag;
		std::string m_branch;
	};

	inline auto toJson(VcsState const& info)
	{
		return KeyValueStore::Compound{}
		   .set("revision", info.revision().c_str())
		   .set("version_tag", info.versionTag().c_str())
		   .set("branch", info.branch().c_str())
		   .takeHandle();
	}
}
#endif
