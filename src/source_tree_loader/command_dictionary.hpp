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
		using result_type = std::pair<fs::path, Db::SourceFileOrigin>;

		CommandDictionary(): m_content{std::make_unique<Content>()}
		{
		}

		result_type get(fs::path const& cmd) const;

		CommandDictionary& projectDir(fs::path&& dir) &
		{
			m_content->m_search_paths[0] = std::move(dir);
			return *this;
		}

		CommandDictionary& systemDir(fs::path&& dir) &
		{
			m_content->m_search_paths[1] = std::move(dir);
			return *this;
		}

		CommandDictionary&& projectDir(fs::path&& dir) &&
		{
			return std::move(projectDir(std::move(dir)));
		}

		CommandDictionary&& systemDir(fs::path&& dir) &&
		{
			return std::move(systemDir(std::move(dir)));
		}

	private:
		struct Content
		{
			std::shared_mutex m_mtx;
			mutable std::map<fs::path, result_type> m_commands;
			std::array<fs::path, 2> m_search_paths;
		};

		std::unique_ptr<Content> m_content;
	};
}
#endif
