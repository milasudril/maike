//@	{
//@	 "targets":[{"name":"source_file.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"source_file.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_SOURCEFILE_HPP
#define MAIKE_SOURCEFILE_HPP

#include "./basic_dependency.hpp"
#include "./compiler.hpp"
#include "./fs.hpp"
#include <vector>

namespace Maike
{
	class SourceFile;

	using Dependency = BasicDependency<SourceFile>;

	class SourceFile
	{
	public:
		/**
		 * Allows to create a node without a source file to be scanned.
		 */
		explicit SourceFile(fs::path const& src);

		explicit SourceFile(fs::path&& src,
		                    std::vector<Dependency>&& used_files,
		                    fs::path const& target_dir,
		                    std::vector<fs::path>&& targets,
		                    Compiler&& compiler);

		bool targetsUpToDate();

		decltype(auto) compileTargets(CompilationLog&)
		{
			// TODO:	return m_compiler.run(m_name, m_used_files, m_targets, log);
			return 0;
		}

		fs::path const& name() const
		{
			return m_name;
		}

		std::vector<fs::path> const& targets() const
		{
			return m_targets;
		}

	private:
		fs::path m_name;
		std::vector<Dependency> m_used_files;
		std::vector<fs::path> m_targets;
		Compiler m_compiler;
	};
}

#endif