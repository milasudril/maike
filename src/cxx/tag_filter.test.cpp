//@	{
//@	 "targets":[{"name":"tag_filter.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./tag_filter.hpp"

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

	class Sink
	{
	public:
		void write(std::byte const* buffer, size_t N)
		{
			auto ptr = reinterpret_cast<char const*>(buffer);
			m_content.insert(std::end(m_content), ptr, ptr + N);
		}

		std::string const& content() const
		{
			return m_content;
		}

	private:
		std::string m_content;
	};

	void write(Sink& sink, std::byte const* buffer, size_t N)
	{
		return sink.write(buffer, N);
	}
}

namespace Testcases
{
	void maikeCxxTagFilterRun()
	{
		Source input{
		   "//@This should be a tag 1\n"
		   "//@     This should be a tag 2\n"
		   "\"//@This should not be a tag\"\n"
		   "Some intial text //@This should not be a tag\n"
		   "R\"raw_string(//@This should not be a tag\n"
		   "//@This should not be a tag\n"
		   ")raw_string\"\n"
		   "text before string literal R\"raw_string(//@This should not be a tag\n"
		   "//@This should not be a tag\n"
		   ")raw_string\"\n"
		   "/*\n"
		   "//@This should not be a tag\n"
		   "*/\n"
		   "Text before comment block/*\n"
		   "//@This should not be a tag\n"
		   "*/"};

		Sink tags;
		Sink source;

		Maike::Cxx::TagFilter{}.run(
		   Maike::Reader{input}, Maike::SourceOutStream{source}, Maike::TagsOutStream{tags});
		assert(source.content() == input.content());

		std::string_view tags_expected{
		   "   This should be a tag 1\n"
		   "        This should be a tag 2\n"
		   "\n"
		   "\n"
		   "\n"
		   "\n"
		   "\n"
		   "\n"
		   "\n"
		   "\n"
		   " \n" // Note: There is an extra whitespace here because the parser cannot know yet whether or
		         // not there will be a valid tag.
		   "\n"
		   "\n"
		   "\n"
		   "\n"
		   ""};
		const auto newlines_in = std::count(std::begin(input.content()), std::end(input.content()), '\n');
		const auto newlines_tags = std::count(std::begin(tags.content()), std::end(tags.content()), '\n');
		const auto newlines_tags_expected =
		   std::count(std::begin(tags.content()), std::end(tags.content()), '\n');

		assert(newlines_in == newlines_tags);
		assert(newlines_in == newlines_tags_expected);
		assert(tags.content() == tags_expected);
	}
}

int main()
{
	Testcases::maikeCxxTagFilterRun();
	return 0;
}