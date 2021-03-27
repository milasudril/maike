//@	{
//@	  "targets":[{"name":"target_info.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_TARGETINFO_HPP
#define MAIKE_DB_TARGETINFO_HPP

#include "src/fs.hpp"

namespace Maike::Db
{
	class TargetInfo
	{
	public:
		explicit TargetInfo(fs::path&& name):
			m_name{std::move(name)}
		{
		}

		fs::path const& name() const { return m_name; }

	private:
		fs::path m_name;
	};
}
#endif