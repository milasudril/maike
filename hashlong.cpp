//@ {
//@	"targets":
//@		[{
//@		 "name":"hashlong.o","type":"object"
//@		,"pkgconfig_libs":["openssl"]
//@		}]
//@	}

#include "hashlong.hpp"
#include "datasource.hpp"
#include <openssl/sha.h>

using namespace Maike;

namespace
	{
	class Sha256Context
		{
		public:
			Sha256Context(std::array<uint8_t,32>& buffer):r_buffer(buffer)
				{SHA256_Init(&m_state);}

			~Sha256Context()
				{SHA256_Final(r_buffer.begin(),&m_state);}

			int update(const void* data,size_t length) noexcept
				{return SHA256_Update(&m_state,data,length);}

		private:
			SHA256_CTX m_state;
			std::array<uint8_t,32>& r_buffer;
		};
	}

std::array<uint8_t,32> Maike::hashlong(DataSource&& src)
	{
	std::array<uint8_t,32> ret;
	static_assert(SHA256_DIGEST_LENGTH==32,"SHA Digest length is wrong");
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
