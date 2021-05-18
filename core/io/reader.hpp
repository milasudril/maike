#ifndef MAIKE_IO_READER_HPP
#define MAIKE_IO_READER_HPP

#include "core/utils/fs.hpp"

#include <cstddef>
#include <type_traits>
#include <cstdint>

namespace Maike::Io
{
	namespace reader_impl
	{
		template<class Source>
		size_t do_read(Source& src, std::byte* buffer, size_t n)
		{
			return read(src, buffer, n);
		}
	}

	class Reader
	{
	public:
		Reader(): r_callback{[](void*, std::byte*, size_t) { return static_cast<size_t>(0); }}
		{
		}

		template<class Source, std::enable_if_t<!std::is_same_v<std::decay_t<Source>, Reader>, int> = 0>
		explicit Reader(Source& src, fs::path const& src_path = fs::path{}):
		   r_source{&src},
		   r_callback{[](void* src, std::byte* buffer, size_t n) {
			   using Src = std::decay_t<Source>;
			   auto& self = *reinterpret_cast<Src*>(src);
			   return static_cast<size_t>(reader_impl::do_read(self, buffer, n));
		   }},
		   m_src_path{std::move(src_path)}
		{
		}

		size_t read(std::byte* buffer, size_t n)
		{
			return r_callback(r_source, buffer, n);
		}

		auto identity() const
		{
			return reinterpret_cast<uintptr_t>(r_source);
		}

		fs::path const& sourcePath() const
		{
			return m_src_path;
		}

	private:
		void* r_source;
		size_t (*r_callback)(void* src, std::byte* buffer, size_t n);
		fs::path m_src_path;
	};

	inline size_t read(Reader reader, std::byte* buffer, size_t n)
	{
		return reader.read(buffer, n);
	}
}

#endif
