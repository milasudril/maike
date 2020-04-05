//@	{
//@	 "targets":[{"name":"source_reader.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./source_reader.hpp"

#include <cassert>
#include <string_view>
#include <string>
#include <cstring>
#include <algorithm>

namespace
{
	class Source
	{
	public:
		explicit Source(std::string_view content):
		   m_content{content},
		   r_read_ptr{m_content.data()},
		   m_n_bytes_left{m_content.size()}
		{
		}

		size_t read(std::byte* buffer, size_t N)
		{
			auto const n = std::min(m_n_bytes_left, N);
			memcpy(buffer, r_read_ptr, n);
			r_read_ptr += n;
			m_n_bytes_left -= n;
			return n;
		}

		std::string_view content() const
		{
			return m_content;
		}

	private:
		std::string_view m_content;
		char const* r_read_ptr;
		size_t m_n_bytes_left;
	};

	size_t read(Source& src, std::byte* buffer, size_t N)
	{
		return src.read(buffer, N);
	}
}

namespace Testcases
{
	void maikeCxxSourceReaderRun()
	{
		Source input{
		   "#include \"foo\"\n"
		   "#include <bar>\n"
		   "\"#include <not_include>\"\n"
		   "Some intial text #include <not_include>\n"
		   "R\"raw_string(<not_include>\n"
		   "<not_include>\n"
		   ")raw_string\"\n"
		   "text before string literal R\"raw_string(#include <not_include>\n"
		   "#include <not_include>\n"
		   ")raw_string\"\n"
		   "/*\n"
		   "#include <not_include>\n"
		   "*/\n"
		   "Text before comment block/*\n"
		   "#include <not_include>\n"
		   "*/"};

		std::vector<Maike::Dependency> deps;

		Maike::Cxx::SourceReader{}.run(Maike::Reader{input}, deps);

		assert(deps.size() == 2);
		assert(deps[0].name() == Maike::fs::path{"foo"});
		assert(deps[1].name() == Maike::fs::path{"bar"});
		assert(deps[0].resolver() == Maike::Dependency::Resolver::InternalLookup);
		assert(deps[1].resolver() == Maike::Dependency::Resolver::None);
	}
}

int main()
{
	Testcases::maikeCxxSourceReaderRun();
	return 0;
}