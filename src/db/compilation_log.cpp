//@	{
//@	  "targets":[{"name":"compilation_log.o","type":"object"}]
//@	 }

#include "./compilation_log.hpp"

#include <algorithm>

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

	std::string shell_escape(std::string const& str)
	{
		std::string ret{"'"};
		std::for_each(std::begin(str), std::end(str), [&ret](auto ch_in) {
			if(ch_in == '\\' || ch_in == '\'') { ret += '\\'; }
			ret += ch_in;
		});
		ret += "'";

		return ret;
	}

	std::string to_string(Maike::Exec::Command const& cmd)
	{
		auto cmdline = shell_escape(cmd.executable.string());
		std::for_each(std::begin(cmd.args), std::end(cmd.args), [&cmdline](auto const& item) {
			cmdline += " ";
			cmdline += shell_escape(item);
		});

		return cmdline;
	}
}

Maike::Db::CompilationLog& Maike::Db::CompilationLog::write(Entry&& e)
{
	auto cmdline = to_string(e.command);
	auto stderr_buff = to_string(e.result.stderr());
	std::lock_guard lock{m_output_mutext};
	fprintf(stdout, "%s\n", cmdline.c_str());
	fprintf(stderr, "%s", stderr_buff.c_str());
	return *this;
}