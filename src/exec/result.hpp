//@	{
//@	 "targets":[{"name":"result.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_EXEC_RESULT_HPP
#define MAIKE_EXEC_RESULT_HPP

namespace Maike::Exec
{
	class Result
	{
	public:
		Result& signo(int s)
		{
			m_signo = s;
			return *this;
		}

		Result& exitStatus(int status)
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

	inline bool failed(Result res)
	{
		if(!res.returnedFromMain()) { return true; }

		return res.exitStatus() != 0;
	}
}

#endif