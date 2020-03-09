//@	{
//@	  "targets":[{"name":"config_store.o","type":"object", "pkgconfig_libs":["jansson"]}]
//@	 }

#include "./config_store.hpp"

namespace
{
	std::string formatError(std::string_view source, int line, int col, char const* description)
	{
		return std::string{source} + ':' + std::to_string(line) + ':' + std::to_string(col + 1) + ": " + description + '.';
	}
}

Maike::ConfigStore::DecodeError::DecodeError(std::string_view source, int line, int col, char const* description):
std::runtime_error{formatError(source, line, col, description)}
{
}