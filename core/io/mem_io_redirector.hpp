#ifndef MAIKE_IO_MEMIOREDIRECTOR_HPP
#define MAIKE_IO_MEMIOREDIRECTOR_HPP

#include "./io_redirector.hpp"

#include <cstddef>
#include <cstring>
#include <vector>

namespace Maike::Io
{
	class MemRedirector
	{
	public:
		explicit MemRedirector(std::byte const* buffer, size_t buffer_size): m_stdin{buffer, buffer_size}
		{
		}

		size_t operator()(std::byte* buffer, size_t n, Redirector::StdIn)
		{
			auto const n_copy = std::min(n, m_stdin.second);
			if(n_copy == 0) { return n_copy; }

			memcpy(buffer, m_stdin.first, n_copy);
			m_stdin.first += n_copy;
			m_stdin.second -= n_copy;
			return n_copy;
		}

		void operator()(std::byte const* buffer, size_t n, Redirector::StdOut)
		{
			m_stdout.insert(std::end(m_stdout), buffer, buffer + n);
		}

		void operator()(std::byte const* buffer, size_t n, Redirector::StdErr)
		{
			m_stderr.insert(std::end(m_stderr), buffer, buffer + n);
		}

		std::vector<std::byte> const& stderr() const
		{
			return m_stderr;
		}

		std::vector<std::byte> const& stdout() const
		{
			return m_stdout;
		}

	private:
		alignas(64) std::pair<std::byte const*, size_t> m_stdin;
		alignas(64) std::vector<std::byte> m_stdout;
		alignas(64) std::vector<std::byte> m_stderr;
	};
}

#endif
