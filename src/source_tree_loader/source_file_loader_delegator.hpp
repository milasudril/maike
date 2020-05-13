//@	{
//@	 "targets":[{"name":"source_file_loader_delegator.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"source_file_loader_delegator.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SOURCEFILELOADERDELEGATOR_HPP
#define MAIKE_SOURCEFILELOADERDELEGATOR_HPP

#include "./source_file_loader.hpp"

#include "src/source_file_info.hpp"
#include "src/fs.hpp"

#include <map>
#include <optional>

namespace Maike::SourceTreeLoader
{
	class SourceFileLoaderDelegator
	{
	public:
		SourceFileLoaderDelegator& loaders(std::map<std::string, SourceFileLoader>&& loaders)
		{
			m_loaders = std::move(loaders);
			return *this;
		}

		std::map<std::string, SourceFileLoader> const& loaders() const
		{
			return m_loaders;
		}

		std::optional<SourceFileInfo> load(Maike::fs::path const& path) const;

	private:
		std::map<std::string, SourceFileLoader> m_loaders;
	};
}

#endif