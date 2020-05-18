//@	{
//@	 "targets":[{"name":"input_buffer.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "input_buffer.hpp"

#include <cassert>
#include <string_view>
#include <cstring>

namespace
{
	class Source
	{
	public:
		Source(std::string_view sv): data{sv}, read_head{data.data()}
		{
		}

		size_t read(std::byte* buffer, size_t N)
		{
			auto const n =
			   std::min(N, std::min(data.size() - (read_head - data.data()), static_cast<size_t>(5)));
			memcpy(buffer, read_head, n);
			read_head += n;
			return n;
		}

	private:
		std::string_view data;
		char const* read_head;
	};

	size_t read(Source& src, std::byte* buffer, size_t N)
	{
		return src.read(buffer, N);
	}
}

namespace Testcases
{
	void maikeInputBufferReadFromSource()
	{
		Maike::Io::InputBuffer test{Source{"Hello, World"}};
		std::string output;
		while(true)
		{
			auto ch_in = test.getchar();
			if(ch_in == -1) { break; }
			output += ch_in;
		}
		assert(output == "Hello, World");
	}
}

int main()
{
	Testcases::maikeInputBufferReadFromSource();
}