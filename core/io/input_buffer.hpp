//@	{
//@	}

#ifndef MAIKE_IO_INPUTBUFFER_HPP
#define MAIKE_IO_INPUTBUFFER_HPP

#include <cstddef>
#include <array>

namespace Maike::Io
{
	template<class Source, size_t Capacity = 4096>
	class InputBuffer
	{
	public:
		InputBuffer(InputBuffer&&) = delete;
		InputBuffer& operator=(InputBuffer&&) = delete;

		explicit InputBuffer(Source src):
		   r_read_ptr{nullptr}, r_read_end{nullptr}, m_source{std::move(src)}
		{
		}

		int getchar()
		{
			if(r_read_ptr == r_read_end)
			{
				fetchAndResetPtr();
				if(r_read_ptr == r_read_end) { return -1; }
			}
			auto ret = *r_read_ptr;
			++r_read_ptr;
			return static_cast<int>(ret);
		}

	private:
		std::byte const* r_read_ptr;
		std::byte const* r_read_end;
		Source m_source;
		std::array<std::byte, Capacity> m_buffer;

		void fetchAndResetPtr();
	};

	template<class Source, size_t Capacity>
	void InputBuffer<Source, Capacity>::fetchAndResetPtr()
	{
		auto n = read(m_source, m_buffer.data(), m_buffer.size());
		if(n == static_cast<decltype(n)>(-1)) { return; }
		r_read_ptr = m_buffer.data();
		r_read_end = m_buffer.data() + n;
	}
}

#endif
