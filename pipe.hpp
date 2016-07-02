//@	{
//@	 "targets":[{"name":"pipe.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"pipe.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_PIPEREADER_HPP
#define MAIKE_PIPEREADER_HPP

#include "datasource.hpp"
#include "datasink.hpp"
#include "twins.hpp"
#include <cstdint>

namespace Maike
	{
	class Pipe
		{
		public:
			static constexpr unsigned int REDIRECT_STDIN=1;
			static constexpr unsigned int REDIRECT_STDOUT=2;
			static constexpr unsigned int REDIRECT_STDERR=4;

			Pipe(const Pipe& rhs)=delete;
			Pipe& operator=(const Pipe& rhs)=delete;

			Pipe(Pipe&& pipe) noexcept;
			Pipe& operator=(Pipe&& pipe) noexcept;

			Pipe(const char* command,Twins<const char* const*> args
				,unsigned int redirection_mask);

			~Pipe();

			int exitStatusGet() noexcept;

			DataSourceHandle stdoutCapture() noexcept
				{return &m_stdout;}

			DataSourceHandle stderrCapture() noexcept
				{return &m_stderr;}

			DataSinkHandle stdinCapture() noexcept
				{return &m_stdin;}

		private:
			class Reader:public DataSource
				{
				public:
					Reader();
					~Reader();
					void* operator new(size_t)=delete;
					void* operator new[](size_t)=delete;
					void operator delete(void *)=delete;
					void operator delete[](void*)=delete;

					size_t read(void* buffer, size_t n);
					const char* nameGet() const noexcept;

					void init(intptr_t handle) noexcept
						{m_handle=handle;}

					void close() noexcept;

				private:
					intptr_t m_handle;
					void destroy() noexcept
						{close();}
				};

			class Writer:public DataSink
				{
				public:
					Writer();
					~Writer();
					void* operator new(size_t)=delete;
					void* operator new[](size_t)=delete;
					void operator delete(void *)=delete;
					void operator delete[](void*)=delete;

					size_t write(const void* buffer, size_t n);

					void init(intptr_t handle) noexcept
						{m_handle=handle;}

					void close() noexcept;

				private:
					intptr_t m_handle;
					void destroy() noexcept
						{close();}
				};

			Writer m_stdin;
			Reader m_stdout;
			Reader m_stderr;
			intptr_t m_pid;
			int m_status;
		};
	}
#endif
