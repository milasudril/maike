//@	{
//@	  "targets":[{"name":"target_info.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"target_info.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_TARGETINFO_HPP
#define MAIKE_DB_TARGETINFO_HPP

#include "./dependency.hpp"

#include "src/fs.hpp"

#include <vector>

namespace Maike::Db
{
	class TargetInfo
	{
	public:
		explicit TargetInfo(fs::path&& name, std::vector<Dependency>&& use_deps):
		   m_name{std::move(name)}, m_use_deps{std::move(use_deps)}
		{
		}

		fs::path const& name() const
		{
			return m_name;
		}

		std::vector<Dependency> const& useDeps() const
		{
			return m_use_deps;
		}

	private:
		fs::path m_name;
		std::vector<Dependency> m_use_deps;
	};

	bool isUpToDate(TargetInfo const& target,
	                fs::path const& src_file,
	                std::vector<Dependency> const& deps);

	// FIXME:
	void compile(std::vector<TargetInfo> const& targets,
	             fs::path const& src_file,
	             std::vector<Dependency> const& deps);
}
#endif