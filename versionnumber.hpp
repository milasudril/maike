//@	{
//@	 "targets":[{"name":"versionnumber.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"versionnumber.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_VERSIONNUMBER_H
#define MAIKE_VERSIONNUMBER_H

#include "visibility.hpp"
#include <cstdint>

namespace Maike
	{
	PRIVATE int64_t version(const char* version_string) noexcept;
	}

#endif
