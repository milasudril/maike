//@	{"targets":[{"name":"targetinclude.hpp","type":"include"}]}

#ifndef MAIKE_TARGETINCLUDE_HPP
#define MAIKE_TARGETINCLUDE_HPP

#define _MAIKE_TARGET(targetdir,x) _STRINGIFY(targetdir/x)
#define _STRINGIFY(x) #x
#define MAIKE_TARGET(x) _MAIKE_TARGET(MAIKE_TARGET_DIRECTORY,x)

#endif
