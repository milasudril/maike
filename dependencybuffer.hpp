//@	{"targets":[{"name":"dependencybuffer.hpp","type":"include"}]}

#ifndef MAIKE_DEPENDENCYBUFFER_HPP
#define MAIKE_DEPENDENCYBUFFER_HPP

#include "visibility.hpp"

namespace Maike
	{
	class Dependency;
	class PRIVATE DependencyBuffer
		{
		public:
			virtual void append(Dependency&& dep)=0;
		};
	}

#endif
