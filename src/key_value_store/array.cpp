//@	{
//@	  "targets":[{"name":"array.o","type":"object"}]
//@	 }

#include "./array.hpp"

Maike::KeyValueStore::Array& Maike::KeyValueStore::operator|=(Array& a, Array const& b)
{
	std::for_each(std::begin(a), std::end(b), [&a](auto const& item) {
		a.append(item);
	});

	return a;
}