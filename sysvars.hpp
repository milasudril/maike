//@	{
//@	 "targets":[{"name":"sysvars.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"sysvars.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SYSVARS_HPP
#define MAIKE_SYSVARS_HPP

#include <map>
#include <string>

namespace Maike
	{
	class Stringkey;
	class Variant;
	void sysvarsLoad(std::map<Stringkey,Variant>& variables
		,std::map<Stringkey,std::string>& strings);
	}

#endif
