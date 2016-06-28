//@	{
//@	 "targets":[ {"name":"maike.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"maike.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_MAIKE_HPP
#define MAIKE_MAIKE_HPP

#include "twins.hpp"
#include <cstddef>

namespace Maike
	{
	class Target;
	class Invoker;

	void buildBranch(Target& target,Invoker& invoker,const char* target_dir
		,size_t targets_count);
	}

#endif
