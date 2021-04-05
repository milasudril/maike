//@	{
//@	 "targets":[{"name":"source_file_load_ctxt.o","type":"object"}]
//@ }

#include "./source_file_load_ctxt.hpp"

#include "src/key_value_store/array.hpp"

#include <cstring>

Maike::Db::Dependency Maike::SourceTreeLoader::getDependency(SourceFileLoadContext const& load_ctxt,
                                                             KeyValueStore::CompoundRefConst dep,
                                                             Db::SourceFileOrigin default_origin)
{
	auto const name = dep.get<char const*>("ref");
	auto expected_origin = default_origin;
	if(auto origin = dep.getIf<char const*>("origin"); origin)
	{
		if(strcmp(*origin, "generated") == 0) { expected_origin = Db::SourceFileOrigin::Generated; }
		else if(strcmp(*origin, "project") == 0)
		{
			expected_origin = Db::SourceFileOrigin::Project;
		}
		else if(strcmp(*origin, "system") == 0)
		{
			expected_origin = Db::SourceFileOrigin::System;
		}
		else if(strcmp(*origin, "pkg-config") == 0)
		{
			expected_origin = Db::SourceFileOrigin::PkgConfig;
		}
	}
	auto ret = Db::Dependency{isExternal(expected_origin) ?
	                             name :
	                             (expected_origin == Db::SourceFileOrigin::Generated ?
	                                 load_ctxt.targetDir() / load_ctxt.sourceFileDir() / name :
	                                 load_ctxt.sourceFileDir() / name),
	                          expected_origin};
	return ret;
}

Maike::Db::TargetInfo
Maike::SourceTreeLoader::getTarget(SourceFileLoadContext const& load_ctxt,
                                   Maike::KeyValueStore::CompoundRefConst target)
{
	auto name = target.template get<char const*>("name");
	std::vector<Db::Dependency> deps;
	if(auto dependencies = target.template getIf<KeyValueStore::ArrayRefConst>("dependencies");
	   dependencies)
	{
		std::transform(std::begin(*dependencies),
		               std::end(*dependencies),
		               std::back_inserter(deps),
		               [&load_ctxt](auto item) {
			               auto obj = item.template as<KeyValueStore::CompoundRefConst>();
			               return getDependency(load_ctxt, obj, Db::SourceFileOrigin::System);
		               });
	}

	if(auto pkgconfig = target.template getIf<KeyValueStore::ArrayRefConst>("pkgconfig_libs");
	   pkgconfig)
	{
		// Put these into the ordinary dependency array. Expand dependency when it is time to
		// compile the source file
		std::transform(
		   std::begin(*pkgconfig), std::end(*pkgconfig), std::back_inserter(deps), [](auto item) {
			   return Db::Dependency{item.template as<char const*>(), Db::SourceFileOrigin::PkgConfig};
		   });
	}
	return Db::TargetInfo{load_ctxt.targetDir() / load_ctxt.sourceFileDir() / name, std::move(deps)};
}

std::vector<Maike::Db::TargetInfo>
Maike::SourceTreeLoader::getTargets(SourceFileLoadContext const& load_ctxt,
                                    Maike::KeyValueStore::Compound const& tags)
{
	std::vector<Db::TargetInfo> ret;
	if(auto target = tags.getIf<KeyValueStore::CompoundRefConst>("target"); target)
	{ ret.push_back(getTarget(load_ctxt, *target)); }

	if(auto targets = tags.getIf<KeyValueStore::ArrayRefConst>("targets"); targets)
	{
		std::transform(
		   std::begin(*targets), std::end(*targets), std::back_inserter(ret), [&load_ctxt](auto item) {
			   auto const val = item.template as<KeyValueStore::CompoundRefConst>();
			   return getTarget(load_ctxt, val);
		   });
	}
	return ret;
}

std::vector<Maike::Db::Dependency>
Maike::SourceTreeLoader::getChildTargetUseDeps(SourceFileLoadContext const& load_ctxt,
                                               KeyValueStore::Compound const& tags)
{
	std::vector<Db::Dependency> ret;

	if(auto deps = tags.getIf<KeyValueStore::ArrayRefConst>("dependencies_extra"); deps)
	{
		std::transform(
		   std::begin(*deps), std::end(*deps), std::back_inserter(ret), [&load_ctxt](auto item) {
			   auto const val = item.template as<KeyValueStore::CompoundRefConst>();
			   return getDependency(load_ctxt, val, Db::SourceFileOrigin::Generated);
		   });
	}
	return ret;
}

std::vector<Maike::Db::Dependency>
Maike::SourceTreeLoader::getUseDeps(SourceFileLoadContext const& load_ctxt,
                                    KeyValueStore::Compound const& tags)
{
	std::vector<Db::Dependency> ret;

	if(auto deps = tags.getIf<KeyValueStore::ArrayRefConst>("dependencies"); deps)
	{
		std::transform(
		   std::begin(*deps), std::end(*deps), std::back_inserter(ret), [&load_ctxt](auto item) {
			   auto const val = item.template as<KeyValueStore::CompoundRefConst>();
			   return getDependency(load_ctxt, val, Db::SourceFileOrigin::Generated);
		   });
	}
	return ret;
}