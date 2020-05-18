//@	{
//@	 "targets":[{"name":"output_buffer.hpp","type":"include"}]
//@	}

#ifndef MAIKE_IO_OUTPUTBUFFER_HPP
#define MAIKE_IO_OUTPUTBUFFER_HPP

#include <cstddef>
#include <array>

namespace Maike::Io
{
	template<class Sink, size_t Capacity = 4096>
	class OutputBuffer
	{
	public:
		OutputBuffer(OutputBuffer&&) = delete;
		OutputBuffer& operator=(OutputBuffer&&) = delete;

		explicit OutputBuffer(Sink sink): r_write_ptr{m_buffer.data()}, m_sink{std::move(sink)}
		{
		}

		OutputBuffer& putchar(std::byte byte)
		{
			if(r_write_ptr == m_buffer.end()) { flush(); }
			*r_write_ptr = byte;
			++r_write_ptr;
			return *this;
		}

		OutputBuffer& putchar(char ch)
		{
			return putchar(static_cast<std::byte>(ch));
		}

		void flush();

		~OutputBuffer()
		{
			flush();
		}

		Sink const& sink() const
		{
			return m_sink;
		}

	private:
		std::byte* r_write_ptr;
		Sink m_sink;
		std::array<std::byte, Capacity> m_buffer;
	};

	template<class Sink, size_t Capacity>
	void OutputBuffer<Sink, Capacity>::flush()
	{
		(void)write(m_sink, m_buffer.data(), static_cast<size_t>(r_write_ptr - m_buffer.data()));
		r_write_ptr = m_buffer.data();
	}
}

#endif