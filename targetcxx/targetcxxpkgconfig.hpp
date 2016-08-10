//@	{
//@	"targets":[{"name":"targetcxxpkgconfig.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxpkgconfig.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXPKGCONFIG_HPP
#define MAIKE_TARGETCXXPKGCONFIG_HPP

#include "../visibility.hpp"

namespace Maike
	{
	class Command;
	class Target;
	class TargetCxxOptions;
 	PRIVATE void pkgconfigAsk(const Command& cmd,const char* libname
		,Target& target,TargetCxxOptions& options);
	}

#endif
