//@	{"targets":[{"name":"textwriter.hpp","type":"include"}]}

#ifndef MAIKE_TEXTWRITER_HPP
#define MAIKE_TEXTWRITER_HPP

namespace Maike
	{
	class TextWriter
		{
		public:
			virtual TextWriter& write(const char* str)=0;
		};
	}

#endif
