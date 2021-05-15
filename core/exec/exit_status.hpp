//@	{
//@	 }

#ifndef MAIKE_EXEC_EXITSTATUS_HPP
#define MAIKE_EXEC_EXITSTATUS_HPP

namespace Maike::Exec
{
	class ExitStatus
	{
	public:
		static ExitStatus success()
		{
			return ExitStatus{}.returnCode(0);
		}

		ExitStatus& signo(int s)
		{
			m_signo = s;
			return *this;
		}

		ExitStatus& returnCode(int status)
		{
			m_signo = -1;
			m_return_code = status;
			return *this;
		}

		int signo() const
		{
			return m_signo;
		}

		int returnCode() const
		{
			return m_return_code;
		}

		bool returnedFromMain() const
		{
			return m_signo == -1;
		};

	private:
		int m_signo;
		int m_return_code;
	};

	inline bool failed(ExitStatus res)
	{
		if(!res.returnedFromMain()) { return true; }

		return res.returnCode() != 0;
	}
}

#endif
