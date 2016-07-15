//@	{
//@	"targets":[{"name":"timedscope.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"timedscope.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TIMEDSCOPE_HPP
#define MAIKE_TIMEDSCOPE_HPP

namespace Maike
	{
	class TimedScope
		{
		public:
			TimedScope(double& T) noexcept;
			~TimedScope() noexcept;

		private:
			double& r_T;
		};
	}

#endif
