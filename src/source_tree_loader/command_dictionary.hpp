//@	{
//@	 "targets":[{"name":"command_dictionary.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"command_dictionary.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SOURCETREELOADER_COMMANDDICTIONARY_HPP
#define MAIKE_SOURCETREELOADER_COMMANDDICTIONARY_HPP

#include "src/utils/fs.hpp"
#include "src/db/source_file_origin.hpp"

#include <map>
#include <shared_mutex>
#include <vector>

namespace Maike::SourceTreeLoader
{
	class CommandDictionary
	{
	public:
		std::pair<Db::SourceFileOrigin, fs::path> get(fs::path const& cmd) const;

		CommandDictionary& projectDir(fs::path&& dir)
		{
			m_search_paths[0] = std::move(dir);
			return *this;
		}

		CommandDictionary& systemDir(fs::path&& dir)
		{
			m_search_paths[1] = std::move(dir);
			return *this;
		}

	private:
		mutable std::shared_mutex m_mtx;
		mutable std::map<fs::path, std::pair<Db::SourceFileOrigin, fs::path>> m_commands;
		std::array<fs::path, 2> m_search_paths;
	};
}
#endif
