//@	{
//@	 "targets":[{"name":"stringutils.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"stringutils.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_STRINGUTILS_HPP
#define MAIKE_STRINGUTILS_HPP

#include <string>
#include <vector>
#include <cstddef>

namespace Maike
{
	std::string toString(std::vector<std::byte> const& src);
}

#endif
