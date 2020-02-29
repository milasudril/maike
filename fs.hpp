//@	{
//@	  "targets":[{"name":"fs.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"stdc++fs", "rel":"external"}]
//@	 }

#ifndef MAIKE_FS_HPP
#define MAIKE_FS_HPP

#include <experimental/filesystem>

namespace Maike
{
	using Path = std::experimental::filesystem::path;
}

#endif