//@	[
//@		[
//@		"posix"
//@			,{"targets":[{"name":"pipe.o","type":"object"}]}
//@		]
//@	]

#include "pipe.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "strerror.hpp"

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <cstdlib>

#include <vector>

#include <cstdio>

using namespace Maike;

namespace
	{
	class PipeHelper
		{
		public:
			PipeHelper(const Pipe&)=delete;
			PipeHelper& operator=(const Pipe&)=delete;

			PipeHelper(){m_fd[0]=-1;m_fd[1]=-1;}

			void init()
				{
				if(pipe(m_fd)==-1)
					{
					exceptionRaise(ErrorMessage("It was not possible to create an IPC pipe. #0;"
						,{static_cast<const char*>(strerror(errno))}));
					}
				}

			~PipeHelper()
				{
				for(int k=0;k<1;++k)
					{
					if(m_fd[k]!=-1)
						{close(m_fd[k]);}
					}
				}

			int readEndRelease() noexcept
				{
				close(m_fd[1]);
				m_fd[1]=-1;

				auto ret=m_fd[0];
				m_fd[0]=-1;
				return ret;
				}

			int readEndGet() noexcept
				{
				close(m_fd[1]);
				m_fd[1]=-1;
				return m_fd[0];
				}

			int writeEndRelease() noexcept
				{
				close(m_fd[0]);
				m_fd[0]=-1;

				auto ret=m_fd[1];
				m_fd[1]=-1;
				return ret;
				}

			int writeEndGet() noexcept
				{
				close(m_fd[0]);
				m_fd[0]=-1;
				return m_fd[1];
				}

		private:
			int m_fd[2];
		};

	class ChildProcess
		{
		public:
			ChildProcess(const ChildProcess&)=delete;
			ChildProcess& operator=(const ChildProcess&)=delete;

			ChildProcess()
				{
				m_pid=fork();
				if(m_pid==-1)
					{
					exceptionRaise(ErrorMessage("It was not possible to create a child process. #0;"
						,{static_cast<const char*>(strerror(errno))}));
					}
				}

			~ChildProcess()
				{
				exitStatusGet();
				}

			pid_t pidGet() const noexcept
				{return m_pid;}

			pid_t pidRelease()
				{
				auto ret=m_pid;
				m_pid=0;
				return ret;
				}


			int exitStatusGet() noexcept
				{
				if(m_pid!=0)
					{
					waitpid(m_pid,&m_status,0);
					m_pid=0;
					}
				return WEXITSTATUS(m_status);
				}



		private:
			pid_t m_pid;
			int m_status;
		};
	}

static std::vector<const char*> commandLineBuild(const char* command
	,Twins<const char* const*> args)
	{
	std::vector<const char*> args_out;
	args_out.push_back(command);
	while(args.first!=args.second)
		{
		args_out.push_back(*args.first);
		++args.first;
		}
	args_out.push_back(nullptr);
	return std::move(args_out);
	}

Pipe::Pipe(const char* command,Twins<const char* const*> args
	,unsigned int redirection_mask)
	{
	PipeHelper stdin_pipe;
	if(redirection_mask&REDIRECT_STDIN)
		{stdin_pipe.init();}

	PipeHelper stdout_pipe;
	if(redirection_mask&REDIRECT_STDOUT)
		{stdout_pipe.init();}

	PipeHelper stderr_pipe;
	if(redirection_mask&REDIRECT_STDERR)
		{stderr_pipe.init();}

	PipeHelper exec_error;
	exec_error.init();

	auto args_out=commandLineBuild(command,args);
	ChildProcess child;
	if(child.pidGet()==0)
		{
	//	In child process
		auto exec_error_write_end=exec_error.writeEndRelease();
		fcntl(exec_error_write_end,F_SETFD,FD_CLOEXEC);

		if(redirection_mask&REDIRECT_STDIN)
			{dup2(stdin_pipe.readEndRelease(),STDIN_FILENO);}

		if(redirection_mask&REDIRECT_STDOUT)
			{dup2(stdout_pipe.writeEndRelease(),STDOUT_FILENO);}

		if(redirection_mask&REDIRECT_STDERR)
			{dup2(stderr_pipe.writeEndRelease(),STDERR_FILENO);}

		if(execvp(command,const_cast<char* const*>(args_out.data()))==-1)
			{
			int status=errno;
			auto res=write(exec_error_write_end,&status,sizeof(status));
			_exit(static_cast<int>(res));
			}
		}

//	Check if execvp failed.
	int status=0;
	if(::read(exec_error.readEndGet(),&status,sizeof(status))==sizeof(status))
		{
		exceptionRaise(ErrorMessage("It was not possible to start #0;. #1;"
			,{command,static_cast<const char*>(strerror(status))}));
		}

	if(redirection_mask&REDIRECT_STDIN)
		{m_stdin.init(stdin_pipe.writeEndRelease());}

	if(redirection_mask&REDIRECT_STDOUT)
		{m_stdout.init(stdout_pipe.readEndRelease());}

	if(redirection_mask&REDIRECT_STDERR)
		{m_stderr.init(stderr_pipe.readEndRelease());}

	m_pid=child.pidRelease();
	}

Pipe::~Pipe()
	{
	exitStatusGet();
	}

int Pipe::exitStatusGet() noexcept
	{
	m_stdin.close();
	m_stdout.close();
	m_stderr.close();
	if(m_pid!=0)
		{
		waitpid(static_cast<int>(m_pid),&m_status,0);
		m_pid=0;
		}
	return WEXITSTATUS(m_status);
	}



Pipe::Reader::Reader():m_handle(-1){}

Pipe::Reader::~Reader()
	{
	close();
	}

void Pipe::Reader::close() noexcept
	{
	if(m_handle!=-1)
		{
		::close(static_cast<int>(m_handle));
		m_handle=-1;
		}
	}

size_t Pipe::Reader::read(void* buffer, size_t count)
	{
	auto pos=reinterpret_cast<uint8_t*>(buffer);
	size_t n_read=0;
	while(n_read!=count)
		{
		auto n=::read(static_cast<int>(m_handle),pos,count-n_read);
		if(n==0)
			{return n_read;}
		if(n==-1)
			{
			if(!( errno==EINTR || errno==EAGAIN || errno==EWOULDBLOCK))
				{exceptionRaise( ErrorMessage("I/O error",{}) );}
			n=0;
			}
		pos+=n;
		n_read+=n;
		}
	return n_read;
	}

const char* Pipe::Reader::nameGet() const noexcept
	{return "Anonymous pipe";}



Pipe::Writer::Writer():m_handle(-1)
	{}

Pipe::Writer::~Writer()
	{close();}

void Pipe::Writer::close() noexcept
	{
	if(m_handle!=-1)
		{
		::close(static_cast<int>(m_handle));
		m_handle=-1;
		}
	}

size_t Pipe::Writer::write(const void* buffer, size_t count)
	{
	if(count==0)
		{return 0;}
	auto bytes=reinterpret_cast<const uint8_t*>(buffer);
	size_t n=0;
	while(n!=count)
		{
		auto res=::write(static_cast<int>(m_handle),bytes,count - n);
		if(res==-1)
			{
			if(errno==ENOSPC)
				{return n;}
			if(!( errno==EINTR || errno==EAGAIN || errno==EWOULDBLOCK))
				{exceptionRaise(ErrorMessage("I/O error",{}));}
			n=0;
			}
		n+=res;
		bytes+=res;
		}
	return n;
	}
