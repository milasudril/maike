//@	{
//@	  "targets":[{"name":"json_handle.o","type":"object", "pkgconfig_libs":["jansson"]}]
//@	 }

#include "./json_handle.hpp"

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
}

bool Maike::KeyValueStore::detail::hasNonWhitespace(std::byte const* begin, std::byte const* end)
{
	return std::any_of(begin, end, [](auto item) {
		auto ch_in = static_cast<char>(item);
		return !(ch_in >= 0 && ch_in <= ' ');
	});
}

Maike::KeyValueStore::JsonHandle Maike::KeyValueStore::detail::jsonLoad(ReadCallbackBase* obj,
                                                                        ReadCallback read_callback,
                                                                        std::string_view src_name)
{
	json_error_t err{};
	Maike::KeyValueStore::JsonHandle ret{
	   json_load_callback(
	      read_callback, obj, JSON_DISABLE_EOF_CHECK | JSON_DECODE_ANY | JSON_REJECT_DUPLICATES, &err),
	   src_name};

	if(!ret.valid() && obj->m_has_data)
	{ throw DecodeError{src_name, err.line, err.column, err.text}; }

	return ret;
}
