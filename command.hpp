//@	{
//@	 "targets":[{"name":"command.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_COMMAND_HPP
#define MAIKE_COMMAND_HPP

#include "./fs.hpp"

namespace Maike
{
	class Command
	{
		public:
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
					{ return m_signo;}

					int exitStatus() const
					{ return m_exit_status; }

					bool returnedFromMain() const
					{ return m_signo == -1;};

				private:
					int m_signo;
					int m_exit_status;
			};

			using StdIn = std::integral_constant<int, 0>;
			using StdOut = std::integral_constant<int, 1>;
			using StdErr = std::integral_constant<int, 2>;

			template<class IoRedirector>
			Result execp(IoRedirector&& io_redir) const
			{
				return execp(&io_redir,
				            [](void* io_redirector, uint8_t* buffer, size_t buffer_size) {
					             auto& self = *reinterpret_cast<IoRedirector*>(io_redirector);
					             self(buffer, buffer_size, StdIn{});
				            }, [](void* io_redirector, uint8_t const* buffer, size_t buffer_size){
					             auto& self = *reinterpret_cast<IoRedirector*>(io_redirector);
					             self(buffer, buffer_size, StdOut{});
				            }, [](void* io_redirector, uint8_t const* buffer, size_t buffer_size){
					             auto& self = *reinterpret_cast<IoRedirector*>(io_redirector);
					             self(buffer, buffer_size, StdErr{});
				            });
			}

			explicit Command(fs::path&& executable, std::vector<std::string>&& args):
			m_executable{std::move(executable)}, m_args{std::move(args)}
			{}

		private:
			using Writer = void(*)(void* io_redirector, uint8_t* buffer, size_t buffer_size);
			using Reader = void(*)(void* io_redirector, uint8_t const* buffer, size_t buffer_size);
			Result execp(void* io_redirector,
			             Writer stdin_writer,
			             Reader stdout_reader,
			             Reader stderr_reader);

			fs::path m_executable;
			std::vector<std::string> m_args;
	};
}

#endif
