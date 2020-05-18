//@	{
//@	 "targets":[{"name":"writer.hpp","type":"include"}]
//@	}

#ifndef MAIKE_IO_WRITER_HPP
#define MAIKE_IO_WRITER_HPP

#include <cstddef>
#include <type_traits>
#include <cstdint>

namespace Maike::Io
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
		Writer(): r_callback{[](void*, std::byte const*, size_t) {}}
		{
		}

		template<class Source, std::enable_if_t<!std::is_same_v<Source, Writer>, int> = 0>
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

		auto identity() const
		{
			return reinterpret_cast<uintptr_t>(r_source);
		}

	private:
		void* r_source;
		void (*r_callback)(void* src, std::byte const* buffer, size_t n);
	};

	inline void write(Writer writer, std::byte const* buffer, size_t n)
	{
		writer.write(buffer, n);
	}

	template<auto Val>
	struct TaggedWriter: public Writer
	{
		static constexpr auto Tag = Val;
		using Writer::Writer;
	};
}

#endif