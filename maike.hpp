//@	{
//@	 "targets":[ {"name":"maike.hpp","type":"include"} ]
//@	,"dependencies_extra":[{"ref":"maike.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_MAIKE_HPP
#define MAIKE_MAIKE_HPP

#include <cstddef>

namespace Maike
	{
	class Target;

	void buildBranch(Target& target,const char* target_dir,size_t targets_count);
	}

#endif
