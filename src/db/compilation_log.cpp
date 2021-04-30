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

	uint64_t to_mus(std::chrono::time_point<std::chrono::steady_clock> t)
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();
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
			stdout_buff += std::to_string(to_mus(e.start_time));
			stdout_buff += " | ";
			stdout_buff += std::to_string(to_mus(e.completion_time));
			[[fallthrough]];
		case LogLevel::Errors: break;
	}

	std::lock_guard lock{m_output_mutext};
	if(stdout_buff.size() > 0) { fprintf(stdout, "%s\n", stdout_buff.c_str()); }
	if(stderr_buff.size() > 0) { fprintf(stderr, "%s\n", stderr_buff.c_str()); }
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