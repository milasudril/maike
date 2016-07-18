//@	{
//@	 "targets":[{"name":"writebuffer.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"writebuffer.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_WRITEBUFFER_HPP
#define MAIKE_WRITEBUFFER_HPP

#include "datasink.hpp"
#include <cstdint>

namespace Maike
	{
	class WriteBuffer
		{
		public:
			WriteBuffer(const WriteBuffer&)=delete;
			WriteBuffer& operator=(const WriteBuffer&)=delete;

			explicit WriteBuffer(DataSink& sink):r_sink(sink)
				{r_write_pos=m_buffer;}

			~WriteBuffer()
				{flush();}

			WriteBuffer& write(uint8_t byte)
				{
				if(bufferFull())
					{flush();}
				*r_write_pos=byte;
				++r_write_pos;
				return *this;
				}

			WriteBuffer& write(const char* str);

			WriteBuffer& write(long long int x);

			WriteBuffer& write(size_t x);

			WriteBuffer& write(double x);

			WriteBuffer& flush()
				{
				r_sink.write(m_buffer,r_write_pos - m_buffer);
				r_write_pos=m_buffer;
				return *this;
				}

		private:
			DataSink& r_sink;
			static constexpr size_t N=4096;
			uint8_t m_buffer[N];
			uint8_t* r_write_pos;

			bool bufferFull() const noexcept
				{return r_write_pos==m_buffer + N;}
		};
	}

#endif
