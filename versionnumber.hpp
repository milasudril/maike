//@	{
//@	 "targets":[{"name":"versionnumber.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"versionnumber.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_VERSIONNUMBER_H
#define MAIKE_VERSIONNUMBER_H

#include <cstdint>

namespace Maike
	{
	int64_t version(const char* version_string) noexcept;
	}

#endif
