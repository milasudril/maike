//@	{
//@	 "targets":[{"name":"command_dictionary.o","type":"object"}]
//@	}

#include "./command_dictionary.hpp"

#include <algorithm>

Maike::SourceTreeLoader::CommandDictionary::result_type
Maike::SourceTreeLoader::CommandDictionary::get(fs::path const& cmd) const
{
	if(cmd == "") { return std::pair{cmd, Db::SourceFileOrigin::Project}; }

	{
		std::shared_lock read_lock{m_content->m_mtx};
		if(auto i = m_content->m_commands.find(cmd); i != std::end(m_content->m_commands))
		{ return i->second; }
	}

	auto res = [&cmd](auto const& search_paths) {
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

		if(i - std::begin(search_paths) == 0)
		{ return std::pair{((*i) / cmd).lexically_normal(), Db::SourceFileOrigin::Project}; }
		return std::pair{((*i) / cmd).lexically_normal(), Db::SourceFileOrigin::System};
	}(m_content->m_search_paths);

	std::lock_guard write_lock{m_content->m_mtx};
	m_content->m_commands[cmd] = res;
	return res;
}