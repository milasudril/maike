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
	std::lock_guard lock{m_output_mutext};
	fprintf(stdout, "%s #", cmdline.c_str());
	fprintf(stdout,
	        " %zu | %s | %zu | %zu\n",
	        e.src_id.value(),
	        e.src_path.c_str(),
	        to_mus(e.start_time),
	        to_mus(e.completion_time));
	fprintf(stderr, "%s", stderr_buff.c_str());
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