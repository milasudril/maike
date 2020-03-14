//@	{
//@	  "targets":[{"name":"json_handle.o","type":"object", "pkgconfig_libs":["jansson"]}]
//@	 }

#include "./json_handle.hpp"

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

Maike::KeyValueStore::JsonHandle Maike::KeyValueStore::detail::jsonLoad(void* obj,
                                                                        ReadCallback read_callback,
                                                                        std::string_view src_name)
{
	json_error_t err{};
	Maike::KeyValueStore::JsonHandle ret{
	   json_load_callback(read_callback, obj, JSON_DISABLE_EOF_CHECK | JSON_DECODE_ANY | JSON_REJECT_DUPLICATES, &err)};

	if(!ret.valid() && !(err.line == 1 && err.column == 0))
	{ throw DecodeError{src_name, err.line, err.column, err.text}; }

	return ret;
}
