//@	{
//@	  "targets":[{"name":"compilation_log.o","type":"object"}]
//@	 }

#include "./compilation_log.hpp"

#include <algorithm>
#include <cinttypes>
#include <cstring>

namespace
{
	std::string to_string(std::vector<std::byte> const& data)
	{
		std::string buffer;
		buffer.reserve(std::size(data));
		std::transform(std::begin(data), std::end(data), std::back_inserter(buffer), [](auto val) {
			return static_cast<char>(val);
		});
		return buffer;
	}

	template<class Rep, class Period>
	uint64_t to_mus(std::chrono::duration<Rep, Period> t)
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(t).count();
	}

	void append_log(std::string& output, std::vector<std::byte> const& data)
	{
		std::for_each(std::begin(data), std::end(data), [&output, newline = true](auto item) mutable{
			if(newline)
			{
				output +="> ";
				newline = false;
			}
			auto ch_in = static_cast<char>(item);
			output += ch_in;
			newline = (ch_in == '\n');
		});
	}
}

Maike::Db::CompilationLog& Maike::Db::CompilationLog::write(Entry&& e)
{
	auto cmdline = toString(e.command);
	auto stderr_buff = to_string(e.result.stderr());
	std::string stdout_buff;
	switch(m_log_level)
	{
		case LogLevel::CompilationCommand:
			stdout_buff += cmdline;
			stdout_buff += " #";
			[[fallthrough]];
		case LogLevel::SourceFileInfo:
			stdout_buff += ' ';
			stdout_buff += std::to_string(e.src_id.value());
			stdout_buff += " | ";
			stdout_buff += e.src_path;
			stdout_buff += " | ";
			stdout_buff += std::to_string(to_mus(e.start_time - m_start_time));
			stdout_buff += " | ";
			stdout_buff += std::to_string(to_mus(e.task_prepared - m_start_time));
			stdout_buff += " | ";
			stdout_buff += std::to_string(to_mus(e.task_ready - m_start_time));
			stdout_buff += " | ";
			stdout_buff += std::to_string(to_mus(e.completion_time - m_start_time));
			[[fallthrough]];
		case LogLevel::Errors: break;
	}

	if(std::size(stdout_buff) > 0) { stdout_buff += "\n"; }
	if(std::size(stderr_buff) > 0) { stderr_buff += "\n"; }

	append_log(stdout_buff, e.result.stdout());

	std::lock_guard lock{m_output_mutex};
	fputs(stdout_buff.c_str(), stdout);
	fputs(stderr_buff.c_str(), stderr);
	return *this;
}

Maike::Db::CompilationLog::OutputFormat
Maike::Db::fromString(Maike::Empty<Maike::Db::CompilationLog::OutputFormat>, char const* str)
{
	if(strcmp(str, "plain_text") == 0) { return CompilationLog::OutputFormat::PlainText; }
	if(strcmp(str, "ansi_term") == 0) { return CompilationLog::OutputFormat::AnsiTerm; }

	throw std::runtime_error{
	   "Unsupported output format. Supported formats: `plain_text`, `ansi_term`"};
}

Maike::Db::CompilationLog::LogLevel
Maike::Db::fromString(Maike::Empty<Maike::Db::CompilationLog::LogLevel>, char const* str)
{
	if(strcmp(str, "errors") == 0) { return CompilationLog::LogLevel::Errors; }
	if(strcmp(str, "source_file_info") == 0) { return CompilationLog::LogLevel::SourceFileInfo; }
	if(strcmp(str, "compilation_command") == 0)
	{ return CompilationLog::LogLevel::CompilationCommand; }

	throw std::runtime_error{
	   "Invalid log level format. Valid log levels: `errors`, `source_file_info`, "
	   "`compilation_command`"};
}

char const* Maike::Db::toString(CompilationLog::OutputFormat format)
{
	using OutputFormat = CompilationLog::OutputFormat;
	switch(format)
	{
		case OutputFormat::PlainText: return "plain_text";

		case OutputFormat::AnsiTerm: return "ansi_term";

		default: __builtin_unreachable();
	}
}

char const* Maike::Db::toString(CompilationLog::LogLevel format)
{
	using LogLevel = CompilationLog::LogLevel;
	switch(format)
	{
		case LogLevel::Errors: return "errors";

		case LogLevel::SourceFileInfo: return "source_file_info";

		case LogLevel::CompilationCommand: return "compilation_command";

		default: __builtin_unreachable();
	}
}