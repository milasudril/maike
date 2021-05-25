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
		std::for_each(std::begin(data), std::end(data), [&output, newline = true](auto item) mutable {
			if(newline)
			{
				output += " ... ";
				newline = false;
			}
			auto ch_in = static_cast<char>(item);
			output += ch_in;
			newline = (ch_in == '\n');
		});
	}
}

Maike::Db::CompilationLog& Maike::Db::CompilationLog::write(Task const& task,
                                                            Exec::Result const& result)
{
	auto cmdline = toString(task.command());
	auto stderr_buff = to_string(result.stderr());
	std::string stdout_buff;
	switch(m_log_level)
	{
		case LogLevel::CompilationCommand:
			stdout_buff += cmdline;
			stdout_buff += " #";
			[[fallthrough]];
		case LogLevel::SourceFileInfo:
		{
			stdout_buff += " ";
			stdout_buff += std::to_string(task.node().id().value());
			stdout_buff += failed(result) ? " | ✗ | " : " | ✓ | ";
			stdout_buff += task.node().path();
			stdout_buff += " | ";
			auto timestamps = task.timestamps();
			stdout_buff += std::to_string(to_mus(timestamps.started - m_start_time));
			stdout_buff += " | ";
			stdout_buff += std::to_string(to_mus(timestamps.completed - m_start_time));
			[[fallthrough]];
		}
		case LogLevel::Errors: break;
	}

	if(std::size(stdout_buff) > 0) { stdout_buff += "\n"; }
	if(std::size(stderr_buff) > 0) { stderr_buff += "\n"; }

	append_log(stdout_buff, result.stdout());

	std::lock_guard lock{m_output_mutex};
	fputs(stdout_buff.c_str(), stdout);
	fputs(stderr_buff.c_str(), stderr);
	return *this;
}

Maike::Db::CompilationLog& Maike::Db::CompilationLog::write(Task const& task)
{
	auto cmdline = toString(task.command());
	std::string stdout_buff;
	switch(m_log_level)
	{
		case LogLevel::CompilationCommand:
			stdout_buff += cmdline;
			stdout_buff += " #";
			[[fallthrough]];
		case LogLevel::SourceFileInfo:
		{
			stdout_buff += " ";
			stdout_buff += std::to_string(task.node().id().value());
			stdout_buff += " | ✓ | ";
			stdout_buff += task.node().path();
			stdout_buff += " | - | - ";
			[[fallthrough]];
		}
		case LogLevel::Errors: break;
	}

	stdout_buff += '\n';

	std::lock_guard lock{m_output_mutex};
	fputs(stdout_buff.c_str(), stdout);
	return *this;
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