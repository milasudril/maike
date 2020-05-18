//@	{"targets":[{"name":"get_state_variables.hpp","type":"include"}]}

#ifndef MAIKE_VCSINVOKER_GETSTATEVARIABLES_HPP
#define MAIKE_VCSINVOKER_GETSTATEVARIABLES_HPP

#include "./config.hpp"

#include "src/io/mem_io_redirector.hpp"
#include "src/utils/stringutils.hpp"

#include <functional>

namespace Maike::VcsInvoker
{
	template<class Invoker>
	std::string getRevision(Config const& cfg, Invoker const& invoker)
	{
		Io::MemRedirector redir{nullptr, 0};
		cfg.getRevision().invoke(invoker, redir);
		return trim(toString(redir.stdout()));
	}

	template<class Invoker>
	std::string getVersionTag(Config const& cfg, Invoker const& invoker)
	{
		Io::MemRedirector redir{nullptr, 0};
		cfg.getVersionTag().invoke(invoker, redir);
		return trim(toString(redir.stdout()));
	}

	template<class Invoker>
	std::string getBranch(Config const& cfg, Invoker const& invoker)
	{
		Io::MemRedirector redir{nullptr, 0};
		cfg.getBranch().invoke(invoker, redir);
		return trim(toString(redir.stdout()));
	}

	template<class Invoker>
	class StateVarsFetcher
	{
	public:
		explicit StateVarsFetcher(std::reference_wrapper<Config const> cfg, Invoker invoker):
		   r_cfg{cfg},
		   m_invoker{std::move(invoker)}
		{
		}

		decltype(auto) revision() const
		{
			return getRevision(r_cfg.get(), m_invoker);
		}

		decltype(auto) versionTag() const
		{
			return getVersionTag(r_cfg.get(), m_invoker);
		}

		decltype(auto) branch() const
		{
			return getBranch(r_cfg.get(), m_invoker);
		}

	private:
		std::reference_wrapper<Config const> r_cfg;
		Invoker m_invoker;
	};

	template<class Invoker>
	StateVarsFetcher<Invoker> getStateVariables(std::reference_wrapper<Config const> cfg,
	                                            Invoker invoker)
	{
		return StateVarsFetcher<Invoker>{cfg, std::forward<Invoker>(invoker)};
	}
}

#endif
