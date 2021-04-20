//@	{
//@	 "targets":[{"name":"command_dictionary.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"command_dictionary.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SOURCETREELOADER_COMMANDDICTIONARY_HPP
#define MAIKE_SOURCETREELOADER_COMMANDDICTIONARY_HPP

#include "src/utils/fs.hpp"

#include <map>
#include <shared_mutex>
#include <vector>

namespace Maike::SourceTreeLoader
{
	class CommandDictionary
	{
	public:
#if 0
		explicit CommandDictionary(std::vector<fs::path>&& search_paths):
		m_search_paths{std::move(search_paths)}
		{}
#endif

		fs::path get(fs::path const& cmd) const;

	private:
		mutable std::shared_mutex m_mtx;
		mutable std::map<fs::path, fs::path> m_commands;
		std::vector<fs::path> m_search_paths;
	};
}
#endif
