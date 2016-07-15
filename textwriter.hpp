
//@	{"targets":[{"name":"textwriter.hpp","type":"include"}]}

#ifndef MAIKE_TEXTWRITER_HPP
#define MAIKE_TEXTWRITER_HPP

#include <cstddef>

namespace Maike
	{
	class TextWriter
		{
		public:
			virtual TextWriter& write(const char* str)=0;
			virtual TextWriter& write(long long int x)=0;
			virtual TextWriter& write(size_t x)=0;
			virtual TextWriter& write(double x)=0;
		};
	}

#endif
