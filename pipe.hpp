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

			Pipe(Pipe&& reader) noexcept;
			Pipe& operator=(Pipe&& reader) noexcept;

			Pipe(const char* command,Twins<const char* const*> args
				,unsigned int redirection_mask);

			~Pipe();

			int exitStatusGet() noexcept;

			DataSource& stdout() noexcept
				{return m_stdout;}

			DataSource& stderr() noexcept
				{return m_stderr;}

			DataSink& stdin() noexcept
				{return m_stdin;}

		private:
			class Reader:public DataSource
				{
				public:
					Reader();
					~Reader();
					size_t read(void* buffer, size_t n);
					const char* nameGet() const noexcept;

					void init(intptr_t handle) noexcept
						{m_handle=handle;}

					void close() noexcept;

				private:
					friend class Pipe;
					intptr_t m_handle;
				};

			class Writer:public DataSink
				{
				public:
					Writer();
					~Writer();
					size_t write(const void* buffer, size_t n);

					void init(intptr_t handle) noexcept
						{m_handle=handle;}

					void close() noexcept;

				private:
					intptr_t m_handle;
				};

			Writer m_stdin;
			Reader m_stdout;
			Reader m_stderr;
			intptr_t m_pid;
			int m_status;
		};
	}
#endif
