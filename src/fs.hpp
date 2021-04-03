//@	{
//@	  "targets":[{"name":"fs.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"fs.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_FS_HPP
#define MAIKE_FS_HPP

#include <filesystem>

namespace Maike
{
	namespace fs = std::filesystem;

	bool newer(fs::path const& a, fs::path const& b);
}

#endif
