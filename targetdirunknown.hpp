//@	{"targets":[{"name":"targetdirunknown.hpp","type":"include"}]}

#ifndef MAIKE_TARGETDIRUNKNOWN_HPP
#define MAIKE_TARGETDIRUNKNOWN_HPP

static_assert(0,"The current target directory is not known by the compiler. "
	"Make sure the macros MAIKE_TARGET_DIRECTORY, and MAIKE_CURRENT_DIRECTORY "
	"has been passed correctly on the command line.");

#endif
