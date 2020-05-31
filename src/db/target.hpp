//@	{
//@	 "targets":[{"name":"target.hpp","type":"include"}]
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
		                std::reference_wrapper<SourceFileInfo const> src_file):
		   m_src_filename{src_file_name},
		   r_src_file{src_file}
		{
		}

		std::vector<Dependency> const& buildDeps() const
		{
			return r_src_file.get().useDeps();
		}

		std::vector<Dependency> buildDepsCopy() const
		{
			return r_src_file.get().useDepsCopy();
		}

		fs::path const& sourceFilename() const
		{
			return m_src_filename;
		}

		/*
		 TODO
		 template<class ... Args>
		 int compile(Args&& ... args) const
		 {
		  return r_src_file.get().compile(std::forward<Args>(args)...);
		 }
		*/
	private:
		fs::path m_src_filename;
		std::reference_wrapper<SourceFileInfo const> r_src_file;
	};
}

#endif