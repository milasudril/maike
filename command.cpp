//@	{
//@	 "targets":[{"name":"command.o","type":"object"}]
//@	 ,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	 }

#include "./command.hpp"

#include <sys/types.h>
#include <unistd.h>

namespace
{
	using Writer = void(*)(void* io_redirector, uint8_t* buffer, size_t buffer_size);
	using Reader = void(*)(void* io_redirector, uint8_t const* buffer, size_t buffer_size);

	Maike::Command::Result communicateWith(pid_t pid,
	                                       void* io_redirector,
	                                       Writer stdin_writer,
	                                       Reader stdout_reader,
                                           Reader stderr_reader)
	{
		return Maike::Command::Result{}.exitStatus(0);
	}
};

Maike::Command::Result Maike::Command::execp(void* io_redirector,
                                             Writer stdin_writer,
                                             Reader stdout_reader,
                                             Reader stderr_reader)
{
	auto pid = fork();
	constexpr decltype(pid) ForkFailed = -1;
	constexpr decltype(pid) Child = 0;

	switch(pid)
	{
		case ForkFailed:
			throw std::runtime_error{"Fork failed"};
		case Child:
			exit(0);
		default:
			return communicateWith(pid, io_redirector, stdin_writer, stdout_reader, stderr_reader);
	}
}
