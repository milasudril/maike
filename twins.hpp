//@	{
//@	"targets":[{"name":"twins.hpp","type":"include"}]
//@	}

#ifndef MAIKE_TWINS_HPP
#define MAIKE_TWINS_HPP

#include "visibility.hpp"
#include <utility>

namespace Maike
	{
	template<class T>
	struct PRIVATE Twins:public std::pair<T,T>
		{
		using std::pair<T,T>::pair;
		};
	}

#endif

