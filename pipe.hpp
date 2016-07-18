//@	{
//@	 "targets":[{"name":"pipe.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"pipe.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_PIPEREADER_HPP
#define MAIKE_PIPEREADER_HPP

#include "datasource.hpp"
#include "datasink.hpp"
#include "twins.hpp"
#include "handle.hpp"
#include "visibility.hpp"
#include <cstdint>

namespace Maike
	{
	class PRIVATE Pipe
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

			~Pipe() noexcept;

			int exitStatusGet() noexcept;

			Handle<DataSource> stdoutCapture() noexcept
				{return Handle<DataSource>( &m_stdout );}

			Handle<DataSource> stderrCapture() noexcept
				{return Handle<DataSource>( &m_stderr );}

			Handle<DataSink> stdinCapture() noexcept
				{return Handle<DataSink>( &m_stdin );}

		private:
			class PRIVATE Reader:public DataSource
				{
				public:
					Reader();
					~Reader() noexcept;
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

			class PRIVATE Writer:public DataSink
				{
				public:
					Writer();
					~Writer() noexcept;
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
