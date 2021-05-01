//@	{
//@	  "targets":[{"name":"compilation_log.o","type":"object"}]
//@	 }

#include "./compilation_log.hpp"

#include <algorithm>
#include <cinttypes>

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
			stdout_buff += std::to_string(to_mus(e.completion_time - m_start_time));
			[[fallthrough]];
		case LogLevel::Errors: break;
	}

	if(std::size(stdout_buff) > 0) {stdout_buff += "\n"; }
	if(std::size(stderr_buff) > 0) {stderr_buff += "\n"; }

	std::lock_guard lock{m_output_mutex};
	fputs(stdout_buff.c_str(), stdout);
	fputs(stderr_buff.c_str(), stderr);
	return *this;
}

Maike::KeyValueStore::JsonHandle Maike::Db::toJson(CompilationLog::OutputFormat format)
{
	using OutputFormat = CompilationLog::OutputFormat;
	switch(format)
	{
		case OutputFormat::PlainText: return KeyValueStore::toJson("plain_text");

		case OutputFormat::AnsiTerm: return KeyValueStore::toJson("ansi_term");

		default: __builtin_unreachable();
	}
};