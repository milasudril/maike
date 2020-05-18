//@	{
//@	 "targets":
//@	 	[{"name":"local_execp.o","type":"object", "dependencies":
//@			[{"ref":"pthread", "rel":"external"}]}]
//@	}

#include "./local_execp.hpp"

#include "src/utils/unused_result.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <algorithm>
#include <thread>
#include <cstddef>
#include <mutex>

namespace
{
	class Pipe
	{
	public:
		Pipe()
		{
			int fds[2];
			if(pipe(fds) == -1) { throw "Failed to create pipe"; }
			m_read_end = fds[0];
			m_write_end = fds[1];
		}

		~Pipe()
		{
			if(m_read_end != -1) { closeReadEnd(); }

			if(m_write_end != -1) { closeWriteEnd(); }
		}

		void write(void const* buffer, size_t count) const
		{
			::write(m_write_end, buffer, count);
		}

		size_t read(void* buffer, size_t count) const
		{
			return ::read(m_read_end, buffer, count);
		}

		void moveReadEndTo(int target)
		{
			::dup2(m_read_end, target);
			closeReadEnd();
			m_read_end = -1;
		}

		void moveWriteEndTo(int target)
		{
			::dup2(m_write_end, target);
			closeWriteEnd();
			m_write_end = -1;
		}

		[[nodiscard]] auto closeReadEndOnExec()
		{
			auto ret = m_read_end;
			::fcntl(m_read_end, F_SETFD, FD_CLOEXEC);
			m_read_end = -1;
			return ret;
		}

		[[nodiscard]] auto closeWriteEndOnExec()
		{
			auto ret = m_write_end;
			::fcntl(m_write_end, F_SETFD, FD_CLOEXEC);
			m_write_end = -1;
			return ret;
		}

		void closeWriteEnd()
		{
			::close(m_write_end);
			m_write_end = -1;
		}

		void closeReadEnd()
		{
			::close(m_read_end);
			m_read_end = -1;
		}

		int readEnd() const
		{
			return m_read_end;
		}

		int writeEnd() const
		{
			return m_write_end;
		}

	private:
		int m_read_end;
		int m_write_end;
	};

	void write(void* io_redirector, int fd, Maike::Io::Redirector::Writer writer)
	{
		constexpr size_t BufferSize = 4096;
		std::array<std::byte, BufferSize> buffer;
		while(true)
		{
			auto n = writer(io_redirector, buffer.data(), BufferSize);
			if(::write(fd, buffer.data(), std::min(n, BufferSize)) == -1) { return; }
			if(n != BufferSize) { return; }
		}
	}

	void read(void* io_redirector, int fd, Maike::Io::Redirector::Reader reader)
	{
		constexpr ssize_t BufferSize = 4096;
		std::array<std::byte, BufferSize> buffer;
		while(true)
		{
			auto n = ::read(fd, buffer.data(), BufferSize);
			if(n == 0 || n == -1) { return; }
			reader(io_redirector, buffer.data(), std::min(BufferSize, n));
		}
	}


	Maike::Exec::Result communicateWith(
	   pid_t pid, Maike::Io::Redirector const& io_redirector, int stdin, int stdout, int stderr)
	{
		signal(SIGPIPE, SIG_IGN);
		std::thread stderr_proc{read, io_redirector.handle(), stderr, io_redirector.stderr()};
		std::thread stdout_proc{read, io_redirector.handle(), stdout, io_redirector.stdout()};
		write(io_redirector.handle(), stdin, io_redirector.stdin());

		int status;
		if(::waitpid(pid, &status, 0) == -1) { abort(); }

		stdout_proc.join();
		stderr_proc.join();

		if(WIFEXITED(status)) { return Maike::Exec::Result{}.exitStatus(WEXITSTATUS(status)); }
		else if(WIFSIGNALED(status))
		{
			return Maike::Exec::Result{}.signo(WTERMSIG(status));
		}

		abort();
	}
};

std::mutex exec_mutex;

Maike::Exec::Result Maike::Exec::execp(fs::path const& executable,
                                       std::vector<std::string> const& args,
                                       Io::Redirector const& io_redirector)
{
	std::vector<char const*> cmd_args;
	cmd_args.push_back(executable.c_str());
	std::transform(std::begin(args),
	               std::end(args),
	               std::back_inserter(cmd_args),
	               [](auto const& item) { return item.c_str(); });
	cmd_args.push_back(nullptr);

	Pipe stdin;
	Pipe stdout;
	Pipe stderr;
	Pipe exec_err;

	std::unique_lock lock{exec_mutex};
	auto pid = ::fork();
	constexpr decltype(pid) ForkFailed = -1;
	constexpr decltype(pid) Child = 0;

	switch(pid)
	{
		case ForkFailed: throw std::runtime_error{"Fork failed"};

		case Child:
			stdin.closeWriteEnd();
			stdout.closeReadEnd();
			stderr.closeReadEnd();
			exec_err.closeReadEnd();

			stdin.moveReadEndTo(STDIN_FILENO);
			stdout.moveWriteEndTo(STDOUT_FILENO);
			stderr.moveWriteEndTo(STDERR_FILENO);
			auto fd_exec_err = exec_err.closeWriteEndOnExec();
			if(execvp(executable.c_str(), const_cast<char* const*>(cmd_args.data())) == -1)
			{
				lock.unlock();
				uint32_t val = errno;
				unusedResult(::write(fd_exec_err, &val, sizeof(errno)));
				::close(fd_exec_err);
				close(STDIN_FILENO);
				close(STDOUT_FILENO);
				close(STDERR_FILENO);
				_exit(-1);
			}
			break;
	}

	exec_err.closeWriteEnd();
	lock.unlock();
	uint32_t val{};
	if(auto n = exec_err.read(&val, sizeof(val)); n == sizeof(val))
	{
		errno = val;
		throw std::runtime_error{"Exec failed"};
	}
	errno = val;

	stdin.closeReadEnd();
	stdout.closeWriteEnd();
	stderr.closeWriteEnd();

	return communicateWith(pid, io_redirector, stdin.writeEnd(), stdout.readEnd(), stderr.readEnd());
}
