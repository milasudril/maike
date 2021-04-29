//@	{
//@	 "targets":[{"name":"result.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_EXEC_RESULT_HPP
#define MAIKE_EXEC_RESULT_HPP

#include "./exit_status.hpp"

#include <vector>
#include <cstddef>

namespace Maike::Exec
{
	class Result
	{
	public:
		explicit Result(ExitStatus status): m_status{status}
		{
		}

		explicit Result(ExitStatus status,
		                std::vector<std::byte>&& stdout,
		                std::vector<std::byte>&& stderr):
		   m_status{status}, m_stdout{std::move(stdout)}, m_stderr{std::move(stderr)}
		{
		}

		ExitStatus exitStatus() const
		{
			return m_status;
		}

		std::vector<std::byte> const& stdout() const
		{
			return m_stdout;
		}
		std::vector<std::byte> const& stderr() const
		{
			return m_stderr;
		}

	private:
		ExitStatus m_status;
		std::vector<std::byte> m_stdout;
		std::vector<std::byte> m_stderr;
	};

	inline bool failed(Result res)
	{
		if(!res.exitStatus().returnedFromMain()) { return true; }

		return res.exitStatus().returnCode() != 0;
	}
}

#endif