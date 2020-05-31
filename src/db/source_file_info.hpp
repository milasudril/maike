//@	{
//@	 "targets":[{"name":"source_file_info.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_SOURCEFILEINFO_HPP
#define MAIKE_DB_SOURCEFILEINFO_HPP

#include "./unresolved_dependency.hpp"

#include "src/compiler.hpp"
#include "src/fs.hpp"

#include <vector>

namespace Maike::Db
{
	class SourceFileInfo
	{
	public:
		/**
		 * Allows to create a node without a source file to be scanned.
		 */
		explicit SourceFileInfo() = default;

		explicit SourceFileInfo(std::vector<UnresolvedDependency>&& build_deps,
		                        std::vector<fs::path>&& targets,
		                        Compiler&& compiler):
		   m_use_deps{std::move(build_deps)},
		   m_targets{std::move(targets)},
		   m_compiler{std::move(compiler)}
		{
		}

		std::vector<UnresolvedDependency> useDepsCopy() const
		{
			return m_use_deps;
		}

		std::vector<UnresolvedDependency> const& useDeps() const
		{
			return m_use_deps;
		}

		SourceFileInfo& add(UnresolvedDependency&& dep)
		{
			m_use_deps.push_back(std::move(dep));
			return *this;
		}

		std::vector<fs::path> const& targets() const
		{
			return m_targets;
		}

		Compiler const& compiler() const
		{
			return m_compiler;
		}

	private:
		std::vector<UnresolvedDependency> m_use_deps;
		std::vector<fs::path> m_targets;
		Compiler m_compiler;
	};
}

#endif