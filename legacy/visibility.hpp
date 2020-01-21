//@	{"targets":[{"name":"visibility.hpp","type":"include"}]}

#ifndef MAIKE_VISIBILITY_HPP
#define MAIKE_VISIBILITY_HPP

#if defined _WIN32 || defined _WIN64
#if MAIKE_EXPORT
	#define EXPORT __declspec(dllexport)
#else
	#define EXPORT __declspec(dllimport)
#endif
	#define PRIVATE
#elif __GNUC__ >=4
	#define EXPORT __attribute__((visibility("default")))
	#define PRIVATE __attribute__((visibility("hidden")))
#else
	#define EXPORT
	#define PRIVATE
#endif

#endif
