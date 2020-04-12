//@	{
//@	  "targets":[{"name":"fs.hpp","type":"include"}]
//	 ,"dependencies_extra":[{"ref":"stdc++fs", "rel":"external"}]
//@	 }

#ifndef MAIKE_FS_HPP
#define MAIKE_FS_HPP

#if 1

#include <filesystem>

namespace Maike
{
	namespace fs = std::filesystem;
}

#else 

#include <experimental/filesystem>

namespace Maike
{
	namespace fs = std::experimental::filesystem;
}

#endif

#endif
