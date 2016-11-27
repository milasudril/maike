//@ {
//@	"targets":
//@		[{
//@		 "name":"hashlong.o","type":"object"
//@		,"pkgconfig_libs":["mhash"]
//@		}]
//@	}

#include "hashlong.hpp"
#include "datasource.hpp"
#include <mhash.h>

using namespace Maike;

namespace
	{
	class Sha256Context
		{
		public:
			Sha256Context(std::array<uint8_t,32>& buffer):r_buffer(buffer)
				{m_state=mhash_init(MHASH_SHA256);}

			~Sha256Context()
				{mhash_deinit(m_state,r_buffer.begin());}

			int update(const void* data,size_t length) noexcept
				{return mhash(m_state,data,length);}

		private:
			MHASH m_state;
			std::array<uint8_t,32>& r_buffer;
		};
	}

std::array<uint8_t,32> Maike::hashlong(DataSource&& src)
	{
	std::array<uint8_t,32> ret;
	Sha256Context ctx(ret);

	std::array<uint8_t,4096> buffer;
	auto n_read=src.read(buffer.begin(),buffer.size());
	while(n_read!=0)
		{
		ctx.update(buffer.begin(),n_read);
		n_read=src.read(buffer.begin(),buffer.size());
		}

	return ret;
	}
