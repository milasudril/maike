//@	{
//@	 }

#ifndef MAIKE_DB_TARGET_HPP
#define MAIKE_DB_TARGET_HPP

#include "./source_file_info.hpp"

namespace Maike::Db
{
	class Target
	{
	public:
		explicit Target(fs::path const& src_file_name,
		                std::reference_wrapper<TargetInfo const> target,
		                std::reference_wrapper<SourceFileInfo const> src_file):
		   m_src_filename{src_file_name}, r_target{target}, r_src_file{src_file}
		{
		}

		std::vector<Dependency> const& buildDeps() const
		{
			return r_src_file.get().useDeps();
		}

		fs::path const& sourceFilename() const
		{
			return m_src_filename;
		}

		std::vector<Dependency> const& useDeps() const
		{
			return r_target.get().useDeps();
		}

	private:
		fs::path m_src_filename;
		std::reference_wrapper<TargetInfo const> r_target;
		std::reference_wrapper<SourceFileInfo const> r_src_file;
	};
}

#endif
