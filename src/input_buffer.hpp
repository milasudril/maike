//@	{
//@	 "targets":[{"name":"input_buffer.hpp","type":"include"}]
//@	}

#ifndef MAIKE_INPUTBUFFER_HPP
#define MAIKE_INPUTBUFFER_HPP

#include <cstddef>
#include <array>

namespace Maike::Cxx
{
	template<class Source, size_t BufferSize = 4096>
	class InputBuffer
	{
		public:
			InputBuffer(InputBuffer&&) = delete;
			InputBUffer& operator=(InputBuffer&&) = delete;

			explicit InputBuffer(Source& src):r_read_ptr{nullptr}, r_read_end{nullptr}, {&src}
			{}

			int getchar()
			{
				if(r_read_ptr == r_read_end)
				{
					fetchAndResetPtr();
					if(r_read_ptr == r_read_end)
					{ return -1;}
				}
				auto ret = *r_read_ptr;
				++r_read_ptr;
				return ret;
			}

		private:
			std::byte const* r_read_ptr;
			std::byte const* r_read_end;
			Source* r_source;
			std::array<std::byte, BufferSize> buffer;

			void fetch();
	};

	template<class Source, size_t BufferSize>
	void InputBuffer::fetchAndResetPtr()
	{
		auto n = read(r_source, buffer.data(), buffer.size());
		if( n == static_cast<decltype(n)>(-1))
		{ return; }
		r_read_ptr = buffer.data();
		r_read_end = buffer.data() + n;
	}
}

#endif