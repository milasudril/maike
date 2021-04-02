//@	{
//@	 "targets":[{"name":"source_file_origin.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_SOURCEFILEORIGIN_HPP
#define MAIKE_DB_SOURCEFILEORIGIN_HPP

namespace Maike::Db
{
	enum class SourceFileOrigin : int
	{
		Project,
		Generated,
		System,
		PkgConfig
	};

	constexpr bool isExternal(SourceFileOrigin origin)
	{
		return origin == SourceFileOrigin::System || origin == SourceFileOrigin::PkgConfig;
	}
}

#endif