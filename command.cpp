//@	{
//@	 "targets":
//@	 	[{"name":"command.o","type":"object", "dependencies":[{"ref":"pthread", "rel":"external"}]}]
//@	}

#include "./command.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <algorithm>
#include <thread>
#include <cstddef>

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

	using Writer = size_t (*)(void* io_redirector, std::byte* buffer, size_t buffer_size);
	using Reader = void (*)(void* io_redirector, std::byte const* buffer, size_t buffer_size);

	void write(void* io_redirector, int fd, Writer writer)
	{
		constexpr size_t BufferSize = 4096;
		std::array<std::byte, BufferSize> buffer;
		while(true)
		{
			auto n = writer(io_redirector, buffer.data(), BufferSize);
			::write(fd, buffer.data(), std::min(n, BufferSize));
			if(n != BufferSize) { return; }
		}
	}

	void read(void* io_redirector, int fd, Reader reader)
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


	Maike::Command::Result communicateWith(pid_t pid,
	                                       void* io_redirector,
	                                       int stdin,
	                                       Writer stdin_writer,
	                                       int stdout,
	                                       Reader stdout_reader,
	                                       int stderr,
	                                       Reader stderr_reader)
	{
		write(io_redirector, stdin, stdin_writer);
		std::thread stdout_proc{read, io_redirector, stdout, stdout_reader};
		std::thread stderr_proc{read, io_redirector, stderr, stderr_reader};

		int status;
		if(::waitpid(pid, &status, 0) == -1) { abort(); }

		stdout_proc.join();
		stderr_proc.join();

		if(WIFEXITED(status)) { return Maike::Command::Result{}.exitStatus(WEXITSTATUS(status)); }
		else if(WIFSIGNALED(status))
		{
			return Maike::Command::Result{}.signo(WTERMSIG(status));
		}

		abort();
	}
};

Maike::Command::Result Maike::Command::execp(void* io_redirector,
                                             Writer stdin_writer,
                                             Reader stdout_reader,
                                             Reader stderr_reader) const
{
	std::vector<char const*> cmd_args;
	cmd_args.push_back(m_executable.c_str());
	std::transform(std::begin(m_args),
	               std::end(m_args),
	               std::back_inserter(cmd_args),
	               [](auto const& item) { return item.c_str(); });
	cmd_args.push_back(nullptr);

	Pipe exec_err;
	Pipe stdout;
	Pipe stderr;
	Pipe stdin;

	auto pid = ::fork();
	constexpr decltype(pid) ForkFailed = -1;
	constexpr decltype(pid) Child = 0;

	switch(pid)
	{
		case ForkFailed: throw std::runtime_error{"Fork failed"};

		case Child:
			exec_err.closeReadEnd();
			stdout.closeReadEnd();
			stderr.closeReadEnd();
			stdin.closeWriteEnd();

			stdout.moveWriteEndTo(STDOUT_FILENO);
			stderr.moveWriteEndTo(STDERR_FILENO);
			stdin.moveReadEndTo(STDIN_FILENO);
			auto fd_exec_err = exec_err.closeWriteEndOnExec();
			if(execvp(m_executable.c_str(), const_cast<char* const*>(cmd_args.data())) == -1)
			{
				uint32_t val = errno;
				::write(fd_exec_err, &val, sizeof(errno));
				::close(fd_exec_err);
				close(STDOUT_FILENO);
				close(STDERR_FILENO);
				close(STDIN_FILENO);
				_exit(-1);
			}
			break;
	}

	exec_err.closeWriteEnd();
	uint32_t val{};
	if(auto n = exec_err.read(&val, sizeof(val)); n == sizeof(val))
	{
		errno = val;
		throw std::runtime_error{"Exec failed"};
	}
	errno = val;
	stdout.closeWriteEnd();
	stderr.closeWriteEnd();
	stdin.closeReadEnd();

	return communicateWith(pid,
	                       io_redirector,
	                       stdin.writeEnd(),
	                       stdin_writer,
	                       stdout.readEnd(),
	                       stdout_reader,
	                       stderr.readEnd(),
	                       stderr_reader);
}
