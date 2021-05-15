//@	{
//@	 "targets":[{"name":"source_file_load_ctxt.o","type":"object"}]
//@ }

#include "./source_file_load_ctxt.hpp"

#include "src/key_value_store/array.hpp"

#include <cstring>

#if 0
Maike::fs::path Maike::SourceTreeLoader::prependSearchPath(SourceFileLoadContext const& load_ctxt,
                                                           fs::path const& src_name,
                                                           Db::SourceFileOrigin expected_origin)
{
	if(expected_origin == Db::SourceFileOrigin::Project
	   || expected_origin == Db::SourceFileOrigin::Generated)
	{
		auto str = src_name.string();
		if(str.size() > 1 && memcmp(str.data(), "./", 2) == 0)
		{ return (load_ctxt.sourceFileDir() / src_name).lexically_normal(); }
		else
		{
			return src_name.lexically_normal();
		}
	}
	return src_name;
}
#else
Maike::fs::path Maike::SourceTreeLoader::prependSearchPath(SourceFileLoadContext const& load_ctxt,
                                                           fs::path const& src_name,
                                                           Db::SourceFileOrigin expected_origin)
{
	if(!(expected_origin == Db::SourceFileOrigin::Project
	     || expected_origin == Db::SourceFileOrigin::Generated))
	{ return src_name; }

	fs::path ret;
	auto str = src_name.string();
	if(str.size() > 1 && memcmp(str.data(), "./", 2) == 0)
	{ ret = (load_ctxt.sourceFileDir() / src_name).lexically_normal(); }
	else
	{
		ret = load_ctxt.sourceDir() / src_name;
	}

	if(expected_origin == Db::SourceFileOrigin::Generated)
	{ ret = ret.lexically_relative(load_ctxt.sourceDir()); }

#if 0
	if(expected_origin == Db::SourceFileOrigin::Project)
	{
		ret = load_ctxt.sourceDir() / ret;
	}
#endif
#if 0
	else
	{
	//	ret = ret.lexically_relative(load_ctxt.sourceDir());
	//	auto tmp = ret.lexically_relative(load_ctxt.sourceDir());
		fprintf(stderr, "%s -> %s\n\n",
		//		load_ctxt.sourceDir().c_str(),
		//		load_ctxt.targetDir().c_str(),
				src_name.c_str(),
				ret.c_str());
	}
#endif

	fprintf(stderr,
	        "(%s) %s %s -> %s\n\n",
	        toString(expected_origin),
	        load_ctxt.sourceFileDir().c_str(),
	        src_name.c_str(),
	        ret.c_str());

	return ret.lexically_normal();
}

#endif


Maike::Db::Dependency
Maike::SourceTreeLoader::prependSearchPath(SourceFileLoadContext const& load_ctxt,
                                           Db::Dependency const& dependency)
{
	auto path = prependSearchPath(load_ctxt, dependency.name(), dependency.expectedOrigin());
	return Db::Dependency{std::move(path),
	                      dependency.expectedOrigin(),
	                      std::vector<Db::Property>{dependency.properties()}};
}

std::vector<Maike::Db::Dependency>
Maike::SourceTreeLoader::prependSearchPath(SourceFileLoadContext const& load_ctxt,
                                           std::vector<Db::Dependency> const& deps)
{
	std::vector<Db::Dependency> ret;
	ret.reserve(deps.size());
	std::transform(std::begin(deps),
	               std::end(deps),
	               std::back_inserter(ret),
	               [&load_ctxt](auto const& item) { return prependSearchPath(load_ctxt, item); });
	return ret;
}

Maike::Db::Dependency Maike::SourceTreeLoader::getDependency(SourceFileLoadContext const& load_ctxt,
                                                             KeyValueStore::CompoundRefConst dep,
                                                             Db::SourceFileOrigin default_origin)
{
	auto const name = dep.get<char const*>("ref");
	auto expected_origin = default_origin;
	if(auto origin = dep.getIf<char const*>("origin"); origin)
	{ expected_origin = fromString(KeyValueStore::Empty<Db::SourceFileOrigin>{}, *origin); }

	std::vector<Db::Property> properties;
	std::for_each(std::begin(dep), std::end(dep), [&properties](auto const& item) {
		if(strcmp(item.first, "ref") != 0 && strcmp(item.first, "origin") != 0)
		{ properties.push_back(Db::Property{item.first, item.second.template as<char const*>()}); }
	});

	auto resolved_name =
	   [](SourceFileLoadContext const& ctxt, fs::path const& name, Db::SourceFileOrigin origin) {
		   if(isExternal(origin)) { return name; }

		   return ((origin == Db::SourceFileOrigin::Generated ? ctxt.targetDir() : ".")
		           / prependSearchPath(ctxt, name, origin))
		      .lexically_normal();
	   }(load_ctxt, name, expected_origin);

	return Db::Dependency{std::move(resolved_name), expected_origin, std::move(properties)};
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
			   return Db::Dependency{item.template as<char const*>(),
			                         Db::SourceFileOrigin::PkgConfig,
			                         std::vector<Db::Property>{}};
		   });
	}
	fprintf(stderr, "%s %s\n", load_ctxt.targetDir().c_str(), load_ctxt.sourceFileDir().c_str());
	auto target_name = (load_ctxt.targetDir()
	                    / (load_ctxt.sourceFileDir() / name).lexically_relative(load_ctxt.sourceDir()))
	                      .lexically_normal();
	fprintf(stderr, "Loaded target %s\n", target_name.c_str());
	return Db::TargetInfo{std::move(target_name), std::move(deps)};
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