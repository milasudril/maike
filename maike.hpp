//@ {
//@ "targets":[ {"name":"maike.hpp","type":"include"} ]
//@ "dependencies_extra":[{"name":"maike.o","rel":"implementation}]
//@ }

#ifndef MAIKE_MAIKE_HPP
#define MAIKE_MAIKE_HPP

#include "twins.hpp"
#include <cstddef>

namespace Maike
	{
	class Target;
	class Invoker;

	void buildBranch(Target& target,Invoker& invoker,size_t targets_count);

	void buildAll(Twins<Target* const*> leafs,Invoker& invoker,size_t targets_count);
	}

#endif
