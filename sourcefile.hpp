//@	{
//@	 "targets":[{"name":"sourcefile.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"sourcefile.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_SOURCEFILE_HPP
#define MAIKE_SOURCEFILE_HPP

#include "compiler.hpp"
#include "fs.hpp"
#include <vector>

namespace Maike
{
	class SourceFile
	{
		public:
			explicit SourceFile(fs::path&& src,
			                    std::vector<fs::path const*>&& used_files,
			                    std::vector<fs::path const*>&& output_files,
			                    Compiler&& compiler);

			bool targetsUpToDate();

			decltype(auto) compileTargets(CompilationLog& log)
			{
				return m_compiler.run(m_name, m_used_files, m_output_files, log);
			}

			fs::path const& name() const
			{
				return m_name;
			}

		private:
			fs::path m_name;
			std::vector<fs::path const*> m_used_files;
			std::vector<fs::path const*> m_output_files;
			Compiler m_compiler;
	};
}

#endif