//@	{
//@	 "targets":[{"name":"command_dictionary.o","type":"object"}]
//@	}

#include "./command_dictionary.hpp"

#include <algorithm>

Maike::Build::CommandDictionary::CommandDictionary()
{
}

Maike::fs::path Maike::Build::CommandDictionary::get(fs::path const& cmd) const
{
	{
		std::shared_lock read_lock{m_mtx};
		if(auto i = m_commands.find(cmd); i != std::end(m_commands)) { return i->second; }
	}

	auto name = [&cmd](auto const& search_paths) {
		auto i = std::find_if(std::begin(search_paths), std::end(search_paths), [&cmd](auto const& item) {
			std::error_code ec;
			return is_regular_file(item / cmd, ec);
		});

		if(i == std::end(search_paths))
		{
			std::string msg{"Faild to map command `"};
			msg += cmd;
			msg += "`";
			throw std::runtime_error{std::move(msg)};
		}

		return (*i) / cmd;
	}(m_search_paths);

	std::lock_guard write_lock{m_mtx};
	m_commands[cmd] = name;
	return name;
}