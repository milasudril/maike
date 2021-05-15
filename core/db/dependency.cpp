//@	{
//@	 "targets":[{"name":"dependency.o","type":"object"}]
//@	}

#include "./dependency.hpp"

#include "core/key_value_store/compound.hpp"

#include <algorithm>

Maike::KeyValueStore::JsonHandle Maike::Db::toJson(Dependency const& dep)
{
	KeyValueStore::Compound ret;
	ret.set("ref", dep.name()).set("origin", toString(dep.expectedOrigin()));

	auto& props = dep.properties();
	std::for_each(std::begin(props), std::end(props), [&ret](auto const& item) {
		ret.set(item.name().c_str(), item.value().c_str());
	});

	return ret.takeHandle();
}