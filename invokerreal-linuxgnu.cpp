//@	[
//@		[
//@		"and(not(less_than(linux,version('2.6.33'))),gnu)"
//@			,{
//@		 	"targets":[
//@				{
//@			 	 "name":"invokerreal.o","type":"object"
//@				,"dependencies":[{"ref":"pthread","rel":"external"}]
//@				}]
//@			}
//@		]
//@	]

#define _LARGEFILE64_SOURCE

#include "invokerreal.hpp"
#include "datasink.hpp"

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
					throw __FILE__;
				//TODO throw ErrorMessage("It was not possible to create an IPC pipe. #0;"
					//	,{strerror(errno)});
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
					throw __FILE__;
				//TODO throw ErrorMessage("It was not possible to create a child process. #0;"
					//	,{strerror(errno)});
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
					throw __FILE__;
				//TODO throw ErrorMessage("It was not possible to start a new thread. #0;"
					//	{strerror(res)});
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
			throw __FILE__;
		//TODO throw ErrorMessage("I/O error",{});
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
	return std::move(str);
	}


static std::string commandLineAsJSON(const char* command
	,Twins<const char* const*> args)
	{
	std::string cmd_json("{\"command\":\"");
	cmd_json+=escape(command)+"\"args\":[";
	while(args.first!=args.second)
		{
		cmd_json+="\""+escape(*args.first);
		++args.first;
		if(args.first!=args.second)
			{cmd_json+=',';}
		}
	cmd_json+="]}";
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
		throw __FILE__;
	//TODO throw ErrorMessage("It was not possible to start #0;. #0;1"
		//	,{command,strerror(status)});
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


bool InvokerReal::newer(const char* file_a,const char* file_b)
	{
	struct stat stat_a;
	struct stat stat_b;

	auto res_a=stat(file_a, &stat_a);
	auto res_b=stat(file_b, &stat_b);

	if(res_a==-1 && res_b==-1)
		{
		throw __FILE__;
	//TODO throw ErrorMessage("None of the files #0;, and #1; are accessible. #2;"
		//	,{strerror(errno)});
		}

	if(res_a==-1)
		{return 0;}

	if(res_b==-1)
		{return 1;}

	return stat_a.st_mtime > stat_b.st_mtime;
	}

void InvokerReal::mkdir(const char* name)
	{
	if( ::mkdir(name, S_IRWXU )==-1 )
		{
		throw __FILE__;
	//TODO throw ErrorMessage("It was not possible to create a directory with name #0;. #0;"
		//	,{strerror(errno)});
		}
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
					throw __FILE__;
				//TODO throw ErrorMessage("It was not possible to open the file #0;. #1;"
					//	,{strerror(errno)});
					}
				}

			FileDescriptor(const char* file,int flags)
				{
				m_fd=open(file,flags);
				if(m_fd==-1)
					{
					throw __FILE__;
				//TODO throw ErrorMessage("It was not possible to open the file #0;. #1;"
					//	,{strerror(errno)});
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

	struct stat source_stat;
	if(fstat(source_fd.get(),&source_stat)==-1)
		{
		unlink(dest);
		throw __FILE__;
	//TODO throw ErrorMessage("stat error: #0;",{strerror(errno)});
		}

	auto size=source_stat.st_size;
	while(size!=0)
		{
		auto res=sendfile(dest_fd.get(),source_fd.get(),nullptr,size);
		if(res==-1)
			{
			unlink(dest);
			throw __FILE__;
		//TODO throw ErrorMessage("It was not possible to copy #0; to #1;"
		//		,{strerror(errno)});
			}

		size-=res;
		}
	}
