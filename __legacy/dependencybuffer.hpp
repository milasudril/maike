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
		virtual void append(Dependency&& dep) = 0;
		virtual const Dependency* begin() const noexcept = 0;
		virtual const Dependency* end() const noexcept = 0;
		virtual void append(const DependencyBuffer& buffer) = 0;
	};
}

#endif
