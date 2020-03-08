//@	{"targets":[{"name":"get_state_variables.hpp","type":"include"}]}

#ifndef MAIKE_VCSINVOKER_GETSTATEVARIABLES_HPP
#define MAIKE_VCSINVOKER_GETSTATEVARIABLES_HPP

#include "./config.hpp"
#include "src/mem_io_redirector.hpp"
#include "src/stringutils.hpp"

namespace Maike::VcsInvoker
{
	template<class Invoker>
	std::string getRevision(Config const& cfg, Invoker const& invoker)
	{
		MemIoRedirector redir{nullptr, 0};
		cfg.getRevision().invoke(invoker, redir);
		return toString(redir.stdout());
	}

	template<class Invoker>
	std::string getVersionTag(Config const& cfg, Invoker const& invoker)
	{
		MemIoRedirector redir{nullptr, 0};
		cfg.getVersionTag().invoke(invoker, redir);
		return toString(redir.stdout());
	}

	template<class Invoker>
	std::string getBranch(Config const& cfg, Invoker const& invoker)
	{
		MemIoRedirector redir{nullptr, 0};
		cfg.getBranch().invoke(invoker, redir);
		return toString(redir.stdout());
	}
}

#endif
