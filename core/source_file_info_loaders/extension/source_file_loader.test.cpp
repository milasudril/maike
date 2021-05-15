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
		   m_content{content}, r_read_ptr{m_content.data()}, m_n_bytes_left{m_content.size()}
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
	void maikeExtensionSourceFileLoaderGetDependencies()
	{
	}

	void maikeExtensionSourceFileLoaderFilterInput()
	{
		Source input{
		   "#@ This should be a tag 1\n"
		   "#@     This should be a tag 2\n"
		   "Some intial text #@This should not be a tag\n"};

		Sink tags;
		Sink source;

		Extension::SourceFileLoader{}.filterInput(Maike::Io::Reader{input},
		                                          Maike::SourceFileInfoLoaders::SourceOutStream{source},
		                                          Maike::SourceFileInfoLoaders::TagsOutStream{tags});
		assert(source.content() == input.content());

		std::string_view tags_expected{
		   "   This should be a tag 1\n"
		   "       This should be a tag 2\n"
		   "\n"};
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
	Testcases::maikeExtensionSourceFileLoaderGetDependencies();
	Testcases::maikeExtensionSourceFileLoaderFilterInput();
	return 0;
}