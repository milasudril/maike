//@ {
//@ "targets":[{"name":"twins.hpp","type":"include"}]
//@ }

#ifndef MAIKE_TWINS_H
#define MAIKE_TWINS_H

#include <utility>

namespace Maike
	{
	template<class T>
	struct Twins:public std::pair<T,T>
		{};
	}

#endif

