//@	{
//@	 "target":{"name":"log_format.o"}
//@	 }

#include "./log_format.hpp"

#include <cstring>

Maike::Db::LogFormat Maike::Db::fromString(Maike::Empty<Maike::Db::LogFormat>, char const* str)
{
	if(strcmp(str, "plain_text") == 0) { return LogFormat::PlainText; }
	if(strcmp(str, "ansi_term") == 0) { return LogFormat::AnsiTerm; }

	throw std::runtime_error{
	   "Unsupported output format. Supported formats: `plain_text`, `ansi_term`"};
}

char const* Maike::Db::toString(LogFormat format)
{
	switch(format)
	{
		case LogFormat::PlainText: return "plain_text";

		case LogFormat::AnsiTerm: return "ansi_term";

		default: __builtin_unreachable();
	}
}