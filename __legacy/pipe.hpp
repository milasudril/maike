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
	class ProcessExitStatus
		{
		public:
			enum class TerminationMode : int {EXITED, KILLED};

			explicit ProcessExitStatus(TerminationMode mode, int error_code):
				m_term_mode{mode}, m_error_code{error_code}
				{}

			int errorCodeGet() const noexcept
				{return m_error_code;}

			bool killedWas() const noexcept
				{return m_term_mode==TerminationMode::KILLED;}

		private:
			TerminationMode m_term_mode;
			int m_error_code;
		};

	inline bool processSucceeded(const ProcessExitStatus& result)
		{return !result.killedWas() && result.errorCodeGet()==0;}

	inline bool processFailed(const ProcessExitStatus& result)
		{return !processSucceeded(result);}

	class PRIVATE Pipe
		{
		public:
			static constexpr unsigned int REDIRECT_STDIN=1;
			static constexpr unsigned int REDIRECT_STDOUT=2;
			static constexpr unsigned int REDIRECT_STDERR=4;
			static constexpr unsigned int ECHO_OFF=8;

			enum class TerminationMode : int{EXITED, KILLED};

			Pipe(const Pipe& rhs)=delete;
			Pipe& operator=(const Pipe& rhs)=delete;

			Pipe(Pipe&& pipe) noexcept;
			Pipe& operator=(Pipe&& pipe) noexcept;

			Pipe(const char* command,Twins<const char* const*> args
				,unsigned int flags);

			~Pipe() noexcept;

			ProcessExitStatus exitStatusGet() noexcept;

			Handle<DataSource> stdoutCapture() noexcept
				{return Handle<DataSource>( &m_stdout );}

			Handle<DataSource> stderrCapture() noexcept
				{return Handle<DataSource>( &m_stderr );}

			Handle<DataSink> stdinCapture() noexcept
				{return Handle<DataSink>( &m_stdin );}

		private:
			class PRIVATE Reader final:public DataSource
				{
				public:
					Reader();
					~Reader() noexcept;
					Reader(const Reader&)=delete;
					Reader& operator=(const Reader&)=delete;
					Reader(Reader&& reader) noexcept;
					Reader& operator=(Reader&& reader) noexcept;

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

			class PRIVATE Writer final:public DataSink
				{
				public:
					Writer();
					~Writer() noexcept;
					Writer(const Writer&)=delete;
					Writer& operator=(const Writer&)=delete;

					Writer(Writer&& writer) noexcept;
					Writer& operator=(Writer&& writer) noexcept;

					void* operator new(size_t)=delete;
					void* operator new[](size_t)=delete;
					void operator delete(void *)=delete;
					void operator delete[](void*)=delete;

					size_t write(const void* buffer, size_t n);

					void init(intptr_t handle,bool echo) noexcept
						{m_handle=handle; m_echo=echo;}

					void close() noexcept;

				private:
					intptr_t m_handle;
					bool m_echo;
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
