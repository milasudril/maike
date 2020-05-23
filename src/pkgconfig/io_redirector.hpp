//@	{
//@	 "targets":[{"name":"io_redirector.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"io_redirector.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_PKGCONFIG_IOREDIRECTOR_HPP
#define MAIKE_PKGCONFIG_IOREDIRECTOR_HPP

#include "src/io/io_redirector.hpp"

#include <string>
#include <vector>

namespace Maike::PkgConfig
{
	class IoRedirector
	{
	public:
		IoRedirector(): m_parser_state{StdOutParserState::Normal}
		{
		}

		std::string const& stderr() const
		{
			return m_stderr;
		}

		std::vector<std::string> takeStdout()
		{
			return std::move(m_stdout);
		}

		size_t operator()(void*, size_t, Io::Redirector::StdIn)
		{
			return 0;
		}

		void operator()(void const* buffer, size_t buffer_size, Io::Redirector::StdOut);

		void operator()(void const* buffer, size_t buffer_size, Io::Redirector::StdErr);

	private:
		enum class StdOutParserState : int
		{
			Normal,
			Escape
		};
		StdOutParserState m_parser_state;
		std::string m_token;
		std::vector<std::string> m_stdout;
		std::string m_stderr;
	};
}

#endif