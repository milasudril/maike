//@ {
//@ "targets":[{"name":"twins.hpp","type":"include"}]
//@ }

#ifndef MAIKE_TWINS_HPP
#define MAIKE_TWINS_HPP

#include <utility>

namespace Maike
	{
	template<class T>
	struct Twins:public std::pair<T,T>
		{
		using std::pair<T,T>::pair;
		};
	}

#endif

