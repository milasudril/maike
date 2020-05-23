//@	{
//@	 "targets":[{"name":"source_file_info.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_SOURCEFILEINFO_HPP
#define MAIKE_SOURCEFILEINFO_HPP

#include "./basic_dependency.hpp"
#include "./compiler.hpp"
#include "./fs.hpp"
#include <vector>

namespace Maike
{
	class SourceFileInfo;

	using Dependency = BasicDependency<SourceFileInfo>;

	class SourceFileInfo
	{
	public:
		/**
		 * Allows to create a node without a source file to be scanned.
		 */
		explicit SourceFileInfo() = default;

		explicit SourceFileInfo(std::vector<Dependency>&& build_deps,
                                      std::vector<fs::path>&& targets,
                                      Compiler&& compiler):
			m_build_deps{std::move(build_deps)},
			m_targets{std::move(targets)},
			m_compiler{std::move(compiler)}
		{
		}

		std::vector<Dependency> buildDepsCopy() const
		{
			return m_build_deps;
		}

		std::vector<Dependency> const& buildDeps() const
		{
			return m_build_deps;
		}

		SourceFileInfo& buildDeps(std::vector<Dependency>&& used_files)
		{
			m_build_deps = std::move(used_files);
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
		std::vector<Dependency> m_build_deps;
		std::vector<fs::path> m_targets;
		Compiler m_compiler;
	};
}

#endif