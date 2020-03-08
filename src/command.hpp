//@	{"targets":[{"name":"command.hpp","type":"include"}]}

#ifndef MAIKE_COMMAND_HPP
#define MAIKE_COMMAND_HPP

#include "./fs.hpp"

#include <string>
#include <vector>

namespace Maike
{
	namespace Command_detail
	{
		template<class Invoker, class IoRedirectorType>
		decltype(auto) execpTrampoline(Invoker const& invoker,
		                               fs::path const& exe,
		                               std::vector<std::string> const& args,
		                               IoRedirectorType&& redir)
		{
			return execp(invoker, exe, args, std::forward<IoRedirectorType>(redir));
		}
	}

	class Command
	{
	public:
		explicit Command(fs::path&& exe, std::vector<std::string>&& args):
		   m_executable{std::move(exe)},
		   m_args{std::move(args)}
		{
		}

		fs::path const& executable() const
		{
			return m_executable;
		}

		std::vector<std::string> const& args() const
		{
			return m_args;
		}

		template<class Invoker, class IoRedirectorType>
		decltype(auto) execp(Invoker const& invoker, IoRedirectorType&& redir)
		{
			return Command_detail::execpTrampoline(
			   invoker, m_executable, m_args, std::forward<IoRedirectorType>(redir));
		}

	private:
		fs::path m_executable;
		std::vector<std::string> m_args;
	};

	bool operator==(Command const& a, Command const& b)
	{
		return a.executable() == b.executable() && a.args() == b.args();
	}

	bool operator!=(Command const& a, Command const& b)
	{
		return !(a == b);
	}
}

#endif
