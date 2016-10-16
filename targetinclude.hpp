//@	{"targets":[{"name":"targetinclude.hpp","type":"include"}]}

#ifndef MAIKE_TARGETINCLUDE_HPP
#define MAIKE_TARGETINCLUDE_HPP

#define _MAIKE_TARGET(targetdir,x) _STRINGIFY(targetdir/x)
#define _STRINGIFY(x) #x

#ifdef MAIKE_TARGET_DIRECTORY
#define MAIKE_TARGET(x) _MAIKE_TARGET(MAIKE_TARGET_DIRECTORY,x)
#else
#define MAIKE_TARGET(x) <maike/targetdirunknown.hpp>
#endif

#endif
