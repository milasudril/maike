//@	{
//@	"dependencies_extra":[{"ref":"./source_file_loader_delegator.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SOURCETREELOADER_SOURCEFILELOADERDELEGATOR_HPP
#define MAIKE_SOURCETREELOADER_SOURCEFILELOADERDELEGATOR_HPP

#include "./command_dictionary.hpp"

#include "core/source_file_info_loaders/loader.hpp"
#include "core/db/source_file_info.hpp"
#include "core/utils/fs.hpp"

#include <map>

namespace Maike::SourceTreeLoader
{
	class SourceFileLoaderDelegator
	{
	public:
		using LoaderMap = std::
		   map<std::string, std::reference_wrapper<SourceFileInfoLoaders::Loader const>, std::less<>>;

		SourceFileLoaderDelegator(std::reference_wrapper<CommandDictionary const> cmds,
		                          std::reference_wrapper<Db::Compiler const> dir_compiler):
		   m_cmds{cmds}, m_dir_compiler{dir_compiler}
		{
		}

		SourceFileLoaderDelegator& loaders(LoaderMap&& loaders)
		{
			m_loaders = std::move(loaders);
			return *this;
		}

		auto const& loaders() const
		{
			return m_loaders;
		}

		Db::SourceFileInfo
		load(fs::path const& path, fs::path const& src_dir, fs::path const& target_dir) const;

	private:
		LoaderMap m_loaders;
		std::reference_wrapper<CommandDictionary const> m_cmds;
		std::reference_wrapper<Db::Compiler const> m_dir_compiler;
	};
}

#endif
