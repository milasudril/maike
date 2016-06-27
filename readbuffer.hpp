//@	{"targets":[{"name":"readbuffer.hpp","type":"include"}]}

#ifndef MAIKE_READBUFFER_HPP
#define MAIKE_READBUFFER_HPP

#include "datasource.hpp"
#include <cstdint>

namespace Maike
	{
	class ReadBuffer
		{
		public:
			ReadBuffer(const ReadBuffer&)=delete;
			ReadBuffer& operator=(const ReadBuffer&)=delete;

			explicit ReadBuffer(DataSource& source):r_source(source)
				{
				r_read_pos=m_buffer;
				m_n_valid=0;
				}

			uint8_t byteRead()
				{
				if(bufferEnd())
					{fetch();}
				auto ret=*r_read_pos;
				++r_read_pos;
				return ret;
				}

			bool eof() const
				{
				if(bufferEnd())
					{fetch();}
				return bufferEnd();
				}

			const char* nameGet() const noexcept
				{return r_source.nameGet();}

		private:
			DataSource& r_source;
			static constexpr size_t N=4096;
			mutable uint8_t m_buffer[N];
			mutable uint8_t* r_read_pos;
			mutable size_t m_n_valid;

			bool bufferEnd() const noexcept
				{return r_read_pos==m_buffer + m_n_valid;}

			void fetch() const
				{
				m_n_valid=r_source.read(m_buffer,N);
				r_read_pos=m_buffer;
				}
		};
	}

#endif
