//@	{
//@	 "targets":[{"name":"io_redirector.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_IO_IOREDIRECTOR_HPP
#define MAIKE_IO_IOREDIRECTOR_HPP

#include <type_traits>
#include <cstddef>

namespace Maike::Io
{
	class Redirector
	{
	public:
		using StdIn = std::integral_constant<int, 0>;
		using StdOut = std::integral_constant<int, 1>;
		using StdErr = std::integral_constant<int, 2>;

		using Writer = size_t (*)(void* io_redirector, std::byte* buffer, size_t buffer_size);
		using Reader = void (*)(void* io_redirector, std::byte const* buffer, size_t buffer_size);

		template<class Callback>
		explicit Redirector(Callback& cb):
		   r_handle{&cb},
		   m_stdin{[](void* io_redirector, std::byte* buffer, size_t buffer_size) {
			   using T = std::decay_t<Callback>;
			   auto& self = *reinterpret_cast<T*>(io_redirector);
			   return self(buffer, buffer_size, Redirector::StdIn{});
		   }},
		   m_stdout{[](void* io_redirector, std::byte const* buffer, size_t buffer_size) {
			   using T = std::decay_t<Callback>;
			   auto& self = *reinterpret_cast<T*>(io_redirector);
			   self(buffer, buffer_size, Redirector::StdOut{});
		   }},
		   m_stderr{[](void* io_redirector, std::byte const* buffer, size_t buffer_size) {
			   using T = std::decay_t<Callback>;
			   auto& self = *reinterpret_cast<T*>(io_redirector);
			   self(buffer, buffer_size, Redirector::StdErr{});
		   }}
		{
		}

		void* handle() const
		{
			return r_handle;
		}

		Writer stdin() const
		{
			return m_stdin;
		}

		Reader stdout() const
		{
			return m_stdout;
		}

		Reader stderr() const
		{
			return m_stderr;
		}

	private:
		void* r_handle;
		Writer m_stdin;
		Reader m_stdout;
		Reader m_stderr;
	};
}

#endif