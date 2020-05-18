//@	{
//@	 "targets":[{"name":"source_file_loader.test","type":"application", "autorun":1}]
//@	}

#undef NDEBUG

#include "./source_file_loader.hpp"

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
	void maikeCxxSourceFileLoaderGetDependencies()
	{
		Source input{
		   "#include \"foo\"\n"
		   "#include <bar>\n"
		   "   #include  \"test1\"\n"
		   "   #include    <test2>\n"
		   "   #include\"test3\"\n"
		   "   #include<test4>\n"
		   "   #  include\"test5\"\n"
		   "   #  include<test6>\n"
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

		auto deps = Cxx::SourceFileLoader{}.getDependencies(Maike::Reader{input});

		assert(deps.size() == 8);
		assert(deps[0].name() == Maike::fs::path{"foo"});
		assert(deps[1].name() == Maike::fs::path{"bar"});
		assert(deps[2].name() == Maike::fs::path{"test1"});
		assert(deps[3].name() == Maike::fs::path{"test2"});
		assert(deps[4].name() == Maike::fs::path{"test3"});
		assert(deps[5].name() == Maike::fs::path{"test4"});
		assert(deps[6].name() == Maike::fs::path{"test5"});
		assert(deps[7].name() == Maike::fs::path{"test6"});
		assert(deps[0].resolver() == Maike::Dependency::Resolver::InternalLookup);
		assert(deps[1].resolver() == Maike::Dependency::Resolver::None);
	}

	void maikeCxxSourceFileLoaderFilterInput()
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

		Cxx::SourceFileLoader{}.filterInput(Maike::Reader{input},
		                                    Maike::SourceTreeLoader::SourceOutStream{source},
		                                    Maike::SourceTreeLoader::TagsOutStream{tags});
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
	Testcases::maikeCxxSourceFileLoaderGetDependencies();
	Testcases::maikeCxxSourceFileLoaderFilterInput();
	return 0;
}