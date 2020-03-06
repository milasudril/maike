//@	{
//@	  "targets":[{"name":"fs.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"stdc++fs", "rel":"external"}]
//@	 }

#ifndef MAIKE_FS_HPP
#define MAIKE_FS_HPP

#include <experimental/filesystem>

namespace Maike
{
	namespace fs = std::experimental::filesystem;
}

#endif