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
			explicit SourceFile(Path&& src,
			                    std::vector<Path const*>&& used_files,
			                    std::vector<Path const*>&& output_files,
			                    Compiler&& compiler);

			bool targetsUpToDate();

			decltype(auto) compileTargets(CompilationLog& log)
			{
				return m_compiler.run(m_name, m_used_files, m_output_files, log);
			}

			Path const& name() const
			{
				return m_name;
			}

		private:
			Path m_name;
			std::vector<Path const*> m_used_files;
			std::vector<Path const*> m_output_files;
			Compiler m_compiler;
	};
}

#endif