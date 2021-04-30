//@	{
//@	 "targets":[{"name":"command.o","type":"object"}]
//@	 }

#include "./command.hpp"

#include <algorithm>

namespace
{
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
}

std::string Maike::Exec::toString(Command const& cmd)
{
	auto cmdline = shell_escape(cmd.executable.string());
	std::for_each(std::begin(cmd.args), std::end(cmd.args), [&cmdline](auto const& item) {
		cmdline += " ";
		cmdline += shell_escape(item);
	});

	return cmdline;
}