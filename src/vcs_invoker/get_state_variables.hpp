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

	template<class Invoker>
	class StateVarsFetcher
	{
	public:
		template<class... Other>
		explicit StateVarsFetcher(Config const&&, Other&&...) = delete;

		explicit StateVarsFetcher(Config const& cfg, Invoker const& invoker):
		   r_cfg{&cfg},
		   r_invoker{&invoker}
		{
		}

		decltype(auto) revision() const
		{
			return getRevision(*r_cfg, *r_invoker);
		}

		decltype(auto) versionTag() const
		{
			return getVersionTag(*r_cfg, *r_invoker);
		}

		decltype(auto) branch() const
		{
			return getBranch(*r_cfg, *r_invoker);
		}

	private:
		Config const* r_cfg;
		Invoker const* r_invoker;
	};

	template<class Invoker>
	StateVarsFetcher<Invoker> getStateVariables(Config const& cfg, Invoker const& invoker)
	{
		return StateVarsFetcher<Invoker>{cfg, invoker};
	}

	template<class Invoker>
	StateVarsFetcher<Invoker> getStateVariables(Config const&& cfg, Invoker&& invoker) = delete;

}

#endif
