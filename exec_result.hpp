//@	{
//@	 "targets":[{"name":"exec_result.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_EXECRSULT_HPP
#define MAIKE_EXECRSULT_HPP

namespace Maike
{
	class ExecResult
	{
	public:
		ExecResult& signo(int s)
		{
			m_signo = s;
			return *this;
		}

		ExecResult& exitStatus(int status)
		{
			m_signo = -1;
			m_exit_status = status;
			return *this;
		}

		int signo() const
		{
			return m_signo;
		}

		int exitStatus() const
		{
			return m_exit_status;
		}

		bool returnedFromMain() const
		{
			return m_signo == -1;
		};

	private:
		int m_signo;
		int m_exit_status;
	};
}

#endif