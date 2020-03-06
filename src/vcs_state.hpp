//@	{"targets":[{"name":"vcs_state.hpp","type":"include"}]}

#ifndef MAIKE_VCSSTATE_HPP
#define MAIKE_VCSSTATE_HPP

#include "src/vcs_invoker/config.hpp"
#include "src/mem_io_redirector.hpp"

namespace Maike
{
	class VcsState
	{
	public:
		template<class Invoker>
		explicit VcsState(VcsInvoker::Config const& cfg, Invoker const& invoker)
		{
		}

	private:
		std::string m_revision;
		std::string m_version_tag;
		std::string m_branch;
	};
}
#endif
