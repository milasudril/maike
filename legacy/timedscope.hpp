//@	{
//@	"targets":[{"name":"timedscope.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"timedscope.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TIMEDSCOPE_HPP
#define MAIKE_TIMEDSCOPE_HPP

#include "visibility.hpp"

namespace Maike
	{
	class PRIVATE TimedScope
		{
		public:
			TimedScope(double& T) noexcept;
			~TimedScope() noexcept;

		private:
			double& r_T;
		};
	}

#endif
