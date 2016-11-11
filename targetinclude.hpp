//@	{"targets":[{"name":"targetinclude.hpp","type":"include"}]}

#ifndef MAIKE_TARGETINCLUDE_HPP
#define MAIKE_TARGETINCLUDE_HPP

#define _MAIKE_TARGET(targetdir,current_dir,x) _STRINGIFY(targetdir/current_dir/x)
#define _STRINGIFY(x) #x

#if defined(MAIKE_TARGET_DIRECTORY) && defined(MAIKE_CURRENT_DIRECTORY)
#define MAIKE_TARGET(x) _MAIKE_TARGET(MAIKE_TARGET_DIRECTORY,MAIKE_CURRENT_DIRECTORY,x)
#else
#define MAIKE_TARGET(x) <maike/targetdirunknown.hpp>
#endif

#endif
