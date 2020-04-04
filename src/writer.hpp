//@	{
//@	 "targets":[{"name":"writer.hpp","type":"include"}]
//@	}

#ifndef MAIKE_WRITER_HPP
#define MAIKE_WRITER_HPP

#include <cstddef>
#include <type_traits>

namespace Maike
{
	namespace writer_impl
	{
		template<class Source>
		void do_write(Source& src, std::byte const* buffer, size_t n)
		{
			write(src, buffer, n);
		}
	}

	class Writer
	{
	public:
		template<class Source>
		explicit Writer(Source& src):
		   r_source{&src},
		   r_callback{[](void* src, std::byte const* buffer, size_t n) {
			   using Src = std::decay_t<Source>;
			   auto& self = *reinterpret_cast<Src*>(src);
			   writer_impl::do_write(self, buffer, n);
		   }}
		{
		}

		void write(std::byte const* buffer, size_t n)
		{
			r_callback(r_source, buffer, n);
		}

	private:
		void* r_source;
		void (*r_callback)(void* src, std::byte const* buffer, size_t n);
	};

	void write(Writer writer, std::byte const* buffer, size_t n)
	{
		writer.write(buffer, n);
	}
}

#endif