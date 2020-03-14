//@	{
//@	  "targets":[{"name":"json_handle.o","type":"object", "pkgconfig_libs":["jansson"]}]
//@	 }

#include "./json_handle.hpp"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdio>

namespace
{
	struct FileDeleter
	{
		void operator()(FILE* f)
		{
			if(f != nullptr) { fclose(f); }
		}
	};

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
	cookie_io_functions_t io_funcs{read_callback, nullptr, nullptr, nullptr};
	std::unique_ptr<FILE, FileDeleter> f{fopencookie(obj, "r", io_funcs)};

	json_error_t err{};
	Maike::KeyValueStore::JsonHandle ret{
	   json_loadf(f.get(), JSON_DISABLE_EOF_CHECK | JSON_DECODE_ANY, &err)};

	if(!ret.valid() && !(err.line == 1 && err.column == 0))
	{ throw DecodeError{src_name, err.line, err.column, err.text}; }

	return ret;
}
