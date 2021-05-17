//@	{
//@	 "targets":[{"name":"source_file_load_ctxt.o","type":"object"}]
//@ }

#include "./source_file_load_ctxt.hpp"

#include "core/key_value_store/array.hpp"
#include "core/io/fifo.hpp"
#include "core/io/input_file.hpp"
#include "core/io/reader.hpp"

#include <cstring>

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
	{ ret = load_ctxt.targetDir() / ret.lexically_relative(load_ctxt.sourceDir()); }

	return ret.lexically_normal();
}

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
		   return prependSearchPath(ctxt, name, origin);
	   }(load_ctxt, name, expected_origin);

	return Db::Dependency{std::move(resolved_name), expected_origin, std::move(properties)};
}

Maike::fs::path Maike::SourceTreeLoader::makeTargetName(SourceFileLoadContext const& load_ctxt,
                                                        fs::path const& name)
{
	return (load_ctxt.targetDir()
	        / (load_ctxt.sourceFileDir() / name).lexically_relative(load_ctxt.sourceDir()))
	   .lexically_normal();
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

	return Db::TargetInfo{makeTargetName(load_ctxt, name), std::move(deps)};
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

Maike::Db::SourceFileInfo
Maike::SourceTreeLoader::loadSourceFile(SourceFileLoadContext const& load_ctxt,
                                        std::vector<Db::Dependency>&& builtin_deps,
                                        SourceFileInfoLoaders::Loader const& loader,
                                        CommandDictionary const&)
{
	Io::Fifo<std::byte> src_fifo;
	Io::Fifo<std::byte> tags_fifo;

	auto const& src_path = load_ctxt.sourcePath();

	Io::InputFile input{src_path};
	loader.filterInput(Io::Reader{input},
	                   SourceFileInfoLoaders::SourceOutStream{src_fifo},
	                   SourceFileInfoLoaders::TagsOutStream{tags_fifo});
	tags_fifo.stop();
	src_fifo.stop();

	{
		auto deps = prependSearchPath(load_ctxt, loader.getDependencies(Io::Reader{src_fifo}));
		builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
	}

	auto tags = KeyValueStore::Compound{Io::Reader{tags_fifo}, src_path.string()};
	auto targets = getTargets(load_ctxt, tags);
	auto use_deps = getUseDeps(load_ctxt, tags);
	auto child_target_use_deps = getChildTargetUseDeps(load_ctxt, tags);

	std::copy(std::begin(use_deps), std::end(use_deps), std::back_inserter(builtin_deps));

	auto compiler = tags.getIf<Db::Compiler>("compiler");
	// TODO: If compiler is valid, it should be looked up in command dictionary
	//       and configured
	[&targets, &builtin_deps, &child_target_use_deps](
	   SourceTreeLoader::SourceFileLoadContext const& load_ctxt,
	   Db::Compiler const& compiler,
	   fs::path const& src_file) {
		auto tags = getTags(compiler, src_file);
		auto script_targets = getTargets(load_ctxt, tags);
		auto use_deps = getUseDeps(load_ctxt, tags);
		auto ctu_deps = getChildTargetUseDeps(load_ctxt, tags);
		std::copy(std::begin(script_targets), std::end(script_targets), std::back_inserter(targets));
		std::copy(std::begin(use_deps), std::end(use_deps), std::back_inserter(builtin_deps));
		std::copy(std::begin(ctu_deps), std::end(ctu_deps), std::back_inserter(child_target_use_deps));
		if(compiler.recipe() != "") { builtin_deps.push_back(makeDependency(compiler)); }
	}(load_ctxt, compiler ? (*compiler) : loader.compiler(), src_path);

	auto const rebuild_policy = [](auto const& tags) {
		if(auto rebuild_policy = tags.template getIf<Db::RebuildPolicy>("rebuild"); rebuild_policy)
		{ return *rebuild_policy; }
		return Db::RebuildPolicy::OnlyIfOutOfDate;
	}(tags);

	return Db::SourceFileInfo{std::move(targets),
	                          std::ref(loader.compiler()),
	                          compiler ? (*compiler) : Db::Compiler{""},
	                          Db::SourceFileOrigin::Project,
	                          rebuild_policy}
	   .useDeps(std::move(builtin_deps))
	   .childTargetsUseDeps(std::move(child_target_use_deps));
}