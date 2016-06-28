//@	{
//@	 "targets":[{"name":"sysvars.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"sysvars.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SYSVARS_HPP
#define MAIKE_SYSVARS_HPP

#include "stringkey.hpp"
#include <map>

namespace Maike
	{
	void sysvarsLoad(std::map<Stringkey,int64_t>& variables);
	}

#endif
