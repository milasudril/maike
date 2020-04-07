//@	{
//@	  "targets":[{"name":"json_handle.o","type":"object", "pkgconfig_libs":["jansson"]}]
//@	 }

#include "./json_handle.hpp"

#include "src/input_buffer.hpp"

#include <algorithm>

namespace
{
	class DecodeError: public std::runtime_error
	{
	public:
		explicit DecodeError(std::string_view source, int line, int col, char const* description):
		   std::runtime_error{std::string{source} + ':' + std::to_string(line) + ':'
		                      + std::to_string(col + 1) + ": " + description + '.'}
		{
		}
	};

	bool hasNonWhitespace(std::byte const* begin, std::byte const* end)
	{
		return std::any_of(begin, end, [](auto item) {
			auto ch_in = static_cast<char>(item);
			return !(ch_in >= 0 && ch_in <= ' ');
		});
	}

	class ReadHelper
	{
	public:
		explicit ReadHelper(Maike::Reader reader): m_buffer{reader}, m_has_data{false}
		{
		}

		size_t fetch(std::byte* buffer, size_t n)
		{
			size_t n_written = 0;
			auto const buffer_init = buffer;
			while(n_written != n)
			{
				auto ch_in = m_buffer.getchar();
				if(ch_in == -1) { return 0; }

				*buffer = static_cast<std::byte>(ch_in);
				++buffer;
				++n_written;

				if(ch_in == '}' || ch_in == ']' || (ch_in >= ' ' && ch_in <= ' ')) { break; }
			}

			if(!hasData()) { m_has_data = hasNonWhitespace(buffer_init, buffer); }

			return n_written;
		}

		bool hasData() const
		{
			return m_has_data;
		}

	private:
		Maike::InputBuffer<Maike::Reader> m_buffer;
		bool m_has_data;
	};
}


Maike::KeyValueStore::JsonHandle Maike::KeyValueStore::jsonLoad(Reader reader,
                                                                std::string_view src_name)
{
	json_error_t err{};
	ReadHelper read_helper{reader};
	Maike::KeyValueStore::JsonHandle ret{
	   json_load_callback(
	      [](void* buffer, size_t buflen, void* data) mutable {
		      auto reader = reinterpret_cast<ReadHelper*>(data);
		      return reader->fetch(reinterpret_cast<std::byte*>(buffer), buflen);
	      },
	      &read_helper,
	      JSON_DISABLE_EOF_CHECK | JSON_DECODE_ANY | JSON_REJECT_DUPLICATES,
	      &err),
	   src_name};

	if(!ret.valid() && read_helper.hasData())
	{ throw DecodeError{src_name, err.line, err.column, err.text}; }

	return ret;
}
