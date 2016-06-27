//@	[
//@		[
//@		"and(not(less_than(linux,version('2.6.33'))),gnu)"
//@			,{
//@		 	"targets":
//@				[{
//@			 	 "name":"invokerreal.o","type":"object"
//@				,"dependencies":[{"ref":"pthread","rel":"external"}]
//@				}]
//@			}
//@		]
//@	]

#define _LARGEFILE64_SOURCE

#include "invokerreal.hpp"
#include "datasinkstd.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "strerror.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>

#include <vector>
#include <cstdint>
#include <exception>
#include <tuple>

using namespace Maike;

namespace
	{
	class Pipe
		{
		public:
			Pipe(const Pipe&)=delete;
			Pipe& operator=(const Pipe&)=delete;

			Pipe()
				{
				if(pipe(m_fd)==-1)
					{
					exceptionRaise(ErrorMessage("It was not possible to create an IPC pipe. #0;"
						,{static_cast<const char*>(strerror(errno))}));
					}
				}

			~Pipe()
				{
				readEndClose();
				writeEndClose();
				}

			int readEndGet() noexcept
				{
				return m_fd[0];
				}

			int writeEndGet() noexcept
				{
				return m_fd[1];
				}

			void writeEndClose() noexcept
				{
				if(m_fd[1]!=-1)
					{
					close(m_fd[1]);
					m_fd[1]=-1;
					}
				}

			void readEndClose() noexcept
				{
				if(m_fd[0]!=-1)
					{
					close(m_fd[0]);
					m_fd[0]=-1;
					}
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

			int pidGet() const noexcept
				{return m_pid;}

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


	class Thread
		{
		public:
			Thread(void *(*start_routine)(void*),void* arg)
				{
				auto res=pthread_create(&m_handle,nullptr,start_routine,arg);
				if(res!=0)
					{
					exceptionRaise(ErrorMessage("It was not possible to start a new thread. #0;"
						,{static_cast<const char*>(strerror(res))}));
					}
				}
			~Thread()
				{
				void* result;
				pthread_join(m_handle,&result);
				}

		private:
			pthread_t m_handle;
		};
	}

static size_t fdRead(int fd,void* buffer,size_t count)
	{
	auto pos=reinterpret_cast<uint8_t*>(buffer);
	size_t n_read=0;
	while(n_read!=count)
		{
		auto n=::read(fd,pos,count-n_read);
		if(n==0)
			{return n_read;}
		if(n==-1)
			{
			exceptionRaise( ErrorMessage("I/O error",{}) );
			}
		pos+=n;
		n_read+=n;
		}
	return n_read;
	}

static std::exception_ptr exceptions[2];

static void* streamPump(void* sp_pair)
	{
	auto data_channel=reinterpret_cast< std::tuple<DataSink*,int,int>* >(sp_pair);

	auto i_except=std::get<2>(*data_channel);
	auto fd=std::get<1>(*data_channel);
	auto& sink=*(std::get<0>(*data_channel));

	try
		{
		static constexpr size_t N=4096;
		size_t n=0;
		char buffer[N];
		do
			{
			n=fdRead(fd,buffer,N);
			sink.write(buffer,n);
			}
		while(n==N);
		return nullptr;
		}
	catch(...)
		{
		exceptions[i_except]=std::current_exception();
		return nullptr;
		}
	}

static std::vector<const char*> commandLineBuild(const char* command
	,Twins<const char* const*> args)
	{
	std::vector<const char*> args_out;
	args_out.push_back(command);
	while(args.first!=args.second)
		{args_out.push_back(*args.first);}
	args_out.push_back(nullptr);
	return std::move(args_out);
	}

static std::string escape(const char* str)
	{
	std::string ret;
	while(*str!='\0')
		{
		if(*str=='"' || *str=='\\')
			{ret+='\\';}
		ret+=*str;
		++str;
		}
	return std::move(ret);
	}


static std::string commandLineAsJSON(const char* command
	,Twins<const char* const*> args)
	{
	std::string cmd_json("{\"method\":\"run\",{\"command\":\"");
	cmd_json+=escape(command)+"\"args\":[";
	while(args.first!=args.second)
		{
		cmd_json+="\""+escape(*args.first);
		++args.first;
		if(args.first!=args.second)
			{cmd_json+=',';}
		}
	cmd_json+="]}}\n,";
	return std::move(cmd_json);
	}

int InvokerReal::run(const char* command,Twins<const char* const*> args
	,DataSink& standard_output,DataSink& standard_error)
	{
	Pipe stdout_pipe;
	Pipe stderr_pipe;
	Pipe exec_error;

	auto args_out=commandLineBuild(command,args);

	auto string_json=commandLineAsJSON(command,args);
	r_echo_stream->write(string_json.c_str(),string_json.size());

	ChildProcess child;


	if(child.pidGet()==0)
		{
	//	In child process
		exec_error.readEndClose();
		fcntl(exec_error.writeEndGet(),FD_CLOEXEC);
		stderr_pipe.readEndClose();
		stdout_pipe.readEndClose();
		dup2(stderr_pipe.writeEndGet(),STDERR_FILENO);
		dup2(stdout_pipe.writeEndGet(),STDOUT_FILENO);

		if(execvp(command,const_cast<char* const*>(args_out.data()))==-1)
			{
			int status=errno;
			auto res=write(exec_error.writeEndGet(),&status,sizeof(status));
			_exit(static_cast<int>(res));
			}
		}

	stdout_pipe.writeEndClose();
	stderr_pipe.writeEndClose();
	exec_error.writeEndClose();

	int status=0;
//	Check if execvp failed.
	if(::read(exec_error.readEndGet(),&status,sizeof(status))==sizeof(status))
		{
		exceptionRaise(ErrorMessage("It was not possible to start #0;. #1;"
			,{command,static_cast<const char*>(strerror(status))}));
		}

	exceptions[0]=nullptr;
	exceptions[1]=nullptr;
		{
		std::tuple<DataSink*,int,int> stdout_channel{&standard_output,stdout_pipe.readEndGet(),0};
		std::tuple<DataSink*,int,int> stderr_channel{&standard_error,stderr_pipe.readEndGet(),1};

		Thread w_a(streamPump,&stdout_channel);
		Thread w_b(streamPump,&stderr_channel);
	//	Wait until both threads has completed
		}

	if(exceptions[0])
		{std::rethrow_exception(exceptions[0]);}
	if(exceptions[1])
		{std::rethrow_exception(exceptions[1]);}


	return child.exitStatusGet();
	}


bool InvokerReal::newer(const char* file_a,const char* file_b) const
	{
	struct stat stat_a;
	struct stat stat_b;

	auto res_a=stat(file_a, &stat_a);
	auto errno_a=errno;
	auto res_b=stat(file_b, &stat_b);
	auto errno_b=errno;

	if(res_a==-1 && res_b==-1)
		{
		exceptionRaise(ErrorMessage("None of the files #0;, and #1; are accessible. #0;: #2;. #1;: #3;."
			,{
			 file_a
			,file_b
			,static_cast<const char*>(strerror(errno_a))
			,static_cast<const char*>(strerror(errno_b))
			}));
		}

	if(res_a==-1)
		{return 0;}

	if(res_b==-1)
		{return 1;}

	return stat_a.st_mtime > stat_b.st_mtime;
	}

void InvokerReal::mkdir(const char* name)
	{
	std::string message("{\"method\":\"mkdir\",{\"name\":\"");
	message+=escape(name);
	message+="\"}}\n,";
	r_echo_stream->write(message.c_str(),message.size());

	if( ::mkdir(name, S_IRWXU )==-1 )
		{
		exceptionRaise(ErrorMessage("It was not possible to create a directory with name #0;. #1;"
			,{name,static_cast<const char*>(strerror(errno))}));
		}
	}


bool InvokerReal::exists(const char* file) const
	{
	if(access(file,F_OK)==-1)
		{return 0;}
	return 1;
	}


namespace
	{
	class FileDescriptor
		{
		public:
			FileDescriptor(const char* file,int flags,int mode)
				{
				m_fd=open(file,flags,mode);
				if(m_fd==-1)
					{
					exceptionRaise(ErrorMessage("It was not possible to open the file #0;. #1;"
						,{static_cast<const char*>(strerror(errno))}));
					}
				}

			FileDescriptor(const char* file,int flags)
				{
				m_fd=open(file,flags);
				if(m_fd==-1)
					{
					exceptionRaise(ErrorMessage("It was not possible to open the file #0;. #1;"
						,{static_cast<const char*>(strerror(errno))}));
					}
				}

			~FileDescriptor()
				{close(m_fd);}

			int get() noexcept
				{return m_fd;}

		private:
			int m_fd;
		};
	}

void InvokerReal::copy(const char* source,const char* dest)
	{
	FileDescriptor source_fd(source,O_RDONLY);
	FileDescriptor dest_fd(dest,O_CREAT|O_WRONLY,S_IRUSR|S_IWUSR);

	std::string message("{\"method\":\"copy\",{\"source\":\"");
	message+=escape(source);
	message+="\",\"dest\":\"";
	message+=escape(dest);
	message+="\"}}\n,";
	r_echo_stream->write(message.c_str(),message.size());


	struct stat source_stat;
	if(fstat(source_fd.get(),&source_stat)==-1)
		{
		unlink(dest);
		exceptionRaise(ErrorMessage("stat error: #0;",{static_cast<const char*>(strerror(errno))}));
		}

	auto size=source_stat.st_size;
	while(size!=0)
		{
		auto res=sendfile(dest_fd.get(),source_fd.get(),nullptr,size);
		if(res==-1)
			{
			unlink(dest);
			exceptionRaise(ErrorMessage("It was not possible to copy #0; to #1;"
				,{static_cast<const char*>(strerror(errno))}));
			}

		size-=res;
		}
	}

InvokerReal::InvokerReal():r_echo_stream(&DataSinkStd::standard_error)
	{}
