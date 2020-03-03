//@	{
//@	 "targets":[{"name":"source_file_info.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"source_file_info.o","rel":"implementation"}]
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

		explicit SourceFileInfo(std::vector<Dependency>&& used_files,
		                        fs::path const& target_dir,
		                        std::vector<fs::path>&& targets,
		                        Compiler&& compiler);

		std::vector<Dependency> const& usedFiles() const
		{
			return m_used_files;
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
		std::vector<Dependency> m_used_files;
		std::vector<fs::path> m_targets;
		Compiler m_compiler;
	};
}

#endif