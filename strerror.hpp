//@	{
//@	 "targets":[{"name":"strerror.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"strerror.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_STRERROR_HPP
#define MAIKE_STRERROR_HPP

#include <cstddef>

namespace Maike
	{
#if _WIN32 || _WIN64
	typedef unsigned int ErrCode;
#else
	typedef int ErrCode;
#endif

	class strerror
		{
		public:
			explicit strerror(ErrCode errcode) noexcept;

			operator const char*() const noexcept
				{return buffer;}

		private:
			static constexpr size_t N=256;
			char buffer[N];
		};
	}

#endif
