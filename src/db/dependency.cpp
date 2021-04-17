//@	{
//@	 "targets":[{"name":"dependency.o","type":"object"}]
//@	}

#include "./dependency.hpp"

#include "src/key_value_store/compound.hpp"

Maike::KeyValueStore::JsonHandle Maike::Db::toJson(Dependency const& dep)
{
#if 0
		SourceFileId m_ref;
		fs::path m_name;
		SourceFileOrigin m_expected_origin;
		std::vector<Property> m_properties;
#endif

	return KeyValueStore::Compound{}
		.set("ref", dep.name().c_str())
//		.set("build_id", info.buildId())
		.takeHandle();
}