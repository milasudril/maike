//@	{
//@	 "targets":[{"name":"sourcefile.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"sourcefile.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_SOURCEFILE_HPP
#define MAIKE_SOURCEFILE_HPP

#include <experimental/filesystem>
#include <vector>
#include <functional>

namespace Maike
{
	using Path = std::experimental::filesystem::path;

	using Compiler = std::function<void(Path const& src,
	                                     std::vector<Path const*> used_files,
	                                     std::vector<Path const*> output_files)>;

	class SourceFile
	{
		public:
			explicit SourceFile(Path&& src);

			bool targetsUpToDate();

			void compileTargets()
			{
				m_compiler(m_name, m_used_files, m_output_files);
			}

		private:
			Path m_name;
			std::vector<Path const*> m_used_files;
			std::vector<Path const*> m_output_files;
			Compiler m_compiler;
	};
}

#endif