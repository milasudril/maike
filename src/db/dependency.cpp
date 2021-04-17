//@	{
//@	 "targets":[{"name":"dependency.o","type":"object"}]
//@	}

#include "./dependency.hpp"

#include "src/key_value_store/compound.hpp"

#include <algorithm>

Maike::KeyValueStore::JsonHandle Maike::Db::toJson(Dependency const& dep)
{
#if 0
		SourceFileId m_ref;
		fs::path m_name;
		SourceFileOrigin m_expected_origin;
		std::vector<Property> m_properties;
#endif

	KeyValueStore::Compound ret;
	ret.set("ref", dep.name());

	auto& props = dep.properties();
	std::for_each(std::begin(props), std::end(props), [&ret](auto const& item) {
		ret.set(item.name().c_str(), item.value().c_str());
	});

	return ret.takeHandle();
}