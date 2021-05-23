//@{"dependencies_extra":[{"ref": "./log_format.o", "rel":"implementation"}]}

#ifndef MAIKE_DB_LOGFORMAT_HPP
#define MAIKE_DB_LOGFORMAT_HPP

#include "core/key_value_store/json_handle.hpp"
#include "core/utils/empty.hpp"

namespace Maike::Db
{
	enum class LogFormat : int
	{
		PlainText,
		AnsiTerm
	};

	LogFormat fromString(Empty<LogFormat>, char const*);

	char const* toString(LogFormat format);

	inline KeyValueStore::JsonHandle toJson(LogFormat format)
	{
		return KeyValueStore::toJson(toString(format));
	}

	constexpr char const* typeToString(Empty<LogFormat>)
	{
		return "Output format";
	}

	constexpr char const* typeDescription(Empty<LogFormat>)
	{
		return "An *Output format* defines how log messages are formatted. Possible options are "
		       "*plain_text*, and "
		       "*ansi_term*."
		       "\n\n"
		       "* *plain_text* will not use any formatting\n\n"
		       "* *ansi_term* uses ANSI escape sequences\n\n"
		       "If *stderr* is a tty, *ansi_term*, is default. Otherwise *plain_text* is the default "
		       "option.";
	}
}

#endif