//@	{
//@	 "targets":[{"name":"source_file_load_ctxt.hpp","type":"include"}]
//@ }

#ifndef MAIKE_SOURCETREELOADER_SOURCEFILELOADCTXT_HPP
#define MAIKE_SOURCETREELOADER_SOURCEFILELOADCTXT_HPP

#include "src/fs.hpp"

namespace Maike::SourceTreeLoader
{
	class SourceFileLoadContext
	{
	public:
		explicit SourceFileLoadContext(fs::path&& source_file_dir,
		                               std::reference_wrapper<fs::path const> target_dir):
		   m_source_file_dir{std::move(source_file_dir)}, m_target_dir{target_dir}
		{
		}


		fs::path const& sourceFileDir() const
		{
			return m_source_file_dir;
		}

		fs::path const& targetDir() const
		{
			return m_target_dir.get();
		}

	private:
		fs::path m_source_file_dir;
		std::reference_wrapper<fs::path const> m_target_dir;
	};
}

#endif