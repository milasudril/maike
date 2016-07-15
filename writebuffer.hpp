//@	{
//@	 "targets":[{"name":"writebuffer.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"writebuffer.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_WRITEBUFFER_HPP
#define MAIKE_WRITEBUFFER_HPP

#include "datasink.hpp"
#include "textwriter.hpp"
#include <cstdint>

namespace Maike
	{
	class WriteBuffer:public TextWriter
		{
		public:
			WriteBuffer(const WriteBuffer&)=delete;
			WriteBuffer& operator=(const WriteBuffer&)=delete;

			explicit WriteBuffer(DataSink& sink):r_sink(sink),n_written_tot(0)
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
				n_written_tot+=r_sink.write(m_buffer,r_write_pos - m_buffer);
				r_write_pos=m_buffer;
				return *this;
				}

			unsigned long long int bytesWrittenGet() const noexcept
				{return n_written_tot;}

		private:
			DataSink& r_sink;
			static constexpr size_t N=4096;
			uint8_t m_buffer[N];
			uint8_t* r_write_pos;
			unsigned long long int n_written_tot;

			bool bufferFull() const noexcept
				{return r_write_pos==m_buffer + N;}
		};
	}

#endif
