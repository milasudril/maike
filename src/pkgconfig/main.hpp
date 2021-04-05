//@	{
//@	 "targets":[{"name":"main.hpp","type":"include"}]
//@	}

#ifndef MAIKE_PKGCONFIG_MAIN_HPP
#define MAIKE_PKGCONFIG_MAIN_HPP

namespace Maike::PkgConfig
{
	std::vector<fs::path> const& incdirs(std::string_view libname);
	std::vector<std::string> const& cflags(std::string_view libname);
	std::vector<fs::path> const& libdirs(std::string_view libname);
	std::vector<fs::path> const& libs(std::string_view libname);
}

#endif
