//@	{
//@	"targets":[{"name":"maikeinfo.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"maikeinfo.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_MAIKEINFO_HPP
#define MAIKE_MAIKEINFO_HPP

namespace Maike
	{
//TODO There should be some kind of version object
	namespace Info
		{
		extern const char* VERSION;
		extern const char* TIMESTAMP;
		}
	}

#endif
