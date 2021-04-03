//@	{
//@	 "targets":[{"name":"source_file_loader_delegator.o","type":"object"}]
//@	}

#include "./source_file_loader_delegator.hpp"

#include "src/io/fifo.hpp"
#include "src/io/input_file.hpp"
#include "src/io/reader.hpp"
#include "src/key_value_store/array.hpp"
#include "src/key_value_store/compound.hpp"

#include <algorithm>
#include <cstring>

namespace
{
	// TODO: These functions should be moved

	class MkDir
	{
	public:
		int run(Maike::fs::path const&,
		        std::vector<Maike::fs::path const*> const&,
		        std::vector<Maike::fs::path const*> const&,
		        Maike::CompilationLog&) const
		{
			return 0;
		}
	};

	std::vector<Maike::Db::Dependency> fixNames(Maike::fs::path const& prefix,
	                                            std::vector<Maike::Db::Dependency> const& deps)
	{
		std::vector<Maike::Db::Dependency> ret;
		ret.reserve(deps.size());
		std::transform(
		   std::begin(deps), std::end(deps), std::back_inserter(ret), [&prefix](auto const& item) {
			   if(item.expectedOrigin() == Maike::Db::SourceFileOrigin::Project)
			   {
				   auto str = item.name().string();
				   if(str.size() > 1 && memcmp(str.data(), "./", 2) == 0)
				   {
					   return Maike::Db::Dependency{(prefix / item.name()).lexically_normal(),
					                                item.expectedOrigin()};
				   }
			   }
			   return item;
		   });
		return ret;
	}

	Maike::Db::Dependency getDependency(Maike::fs::path const& src_dir,
	                                    Maike::KeyValueStore::CompoundRefConst dep,
	                                    Maike::Db::SourceFileOrigin default_origin)
	{
		// FIXME: Need target dir here
		auto const name = dep.get<char const*>("ref");
		auto expected_origin = default_origin;
		if(auto origin = dep.getIf<char const*>("origin"); origin)
		{
			if(strcmp(*origin, "generated") == 0)
			{ expected_origin = Maike::Db::SourceFileOrigin::Generated; }
			else if(strcmp(*origin, "project") == 0)
			{
				expected_origin = Maike::Db::SourceFileOrigin::Project;
			}
			else if(strcmp(*origin, "system") == 0)
			{
				expected_origin = Maike::Db::SourceFileOrigin::System;
			}
			else if(strcmp(*origin, "pkg-config") == 0)
			{
				expected_origin = Maike::Db::SourceFileOrigin::PkgConfig;
			}
		}
		auto ret = Maike::Db::Dependency{isExternal(expected_origin) ?
		                                    name :
		                                    expected_origin == Maike::Db::SourceFileOrigin::Generated ?
		                                    Maike::fs::path{"__targets"} / src_dir / name :
		                                    src_dir / name,
		                                 expected_origin};
		return ret;
	}

	Maike::Db::TargetInfo getTarget(Maike::fs::path const& src_dir,
	                                Maike::KeyValueStore::CompoundRefConst target)
	{
		auto name = target.template get<char const*>("name");
		std::vector<Maike::Db::Dependency> deps;
		if(auto dependencies = target.template getIf<Maike::KeyValueStore::ArrayRefConst>("dependencies");
		   dependencies)
		{
			std::transform(std::begin(*dependencies),
			               std::end(*dependencies),
			               std::back_inserter(deps),
			               [&src_dir](auto item) {
				               auto obj = item.template as<Maike::KeyValueStore::CompoundRefConst>();
				               return getDependency(src_dir, obj, Maike::Db::SourceFileOrigin::System);
			               });
		}

		if(auto pkgconfig = target.template getIf<Maike::KeyValueStore::ArrayRefConst>("pkgconfig_libs");
		   pkgconfig)
		{
			// Put these into the ordinary dependency array. Expand dependency when it is time to
			// compile the source file
			std::transform(
			   std::begin(*pkgconfig), std::end(*pkgconfig), std::back_inserter(deps), [](auto item) {
				   return Maike::Db::Dependency{item.template as<char const*>(),
				                                Maike::Db::SourceFileOrigin::PkgConfig};
			   });
		}
		return Maike::Db::TargetInfo{src_dir / name, std::move(deps)};
	}


	std::vector<Maike::Db::TargetInfo> getTargets(Maike::fs::path const& src_dir,
	                                              Maike::KeyValueStore::Compound const& tags)
	{
		std::vector<Maike::Db::TargetInfo> ret;
		if(auto target = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("target"); target)
		{ ret.push_back(getTarget(src_dir, *target)); }

		if(auto targets = tags.getIf<Maike::KeyValueStore::ArrayRefConst>("targets"); targets)
		{
			std::transform(
			   std::begin(*targets), std::end(*targets), std::back_inserter(ret), [&src_dir](auto item) {
				   auto const val = item.template as<Maike::KeyValueStore::CompoundRefConst>();
				   return getTarget(src_dir, val);
			   });
		}
		return ret;
	}

	std::vector<Maike::Db::Dependency>
	getChildTargetUseDeps(Maike::fs::path const& src_dir, Maike::KeyValueStore::Compound const& tags)
	{
		std::vector<Maike::Db::Dependency> ret;

		if(auto deps = tags.getIf<Maike::KeyValueStore::ArrayRefConst>("dependencies_extra"); deps)
		{
			std::transform(
			   std::begin(*deps), std::end(*deps), std::back_inserter(ret), [&src_dir](auto item) {
				   auto const val = item.template as<Maike::KeyValueStore::CompoundRefConst>();
				   return getDependency(src_dir, val, Maike::Db::SourceFileOrigin::Generated);
			   });
		}
		return ret;
	}

	std::vector<Maike::Db::Dependency> getUseDeps(Maike::fs::path const& src_dir,
	                                              Maike::KeyValueStore::Compound const& tags)
	{
		std::vector<Maike::Db::Dependency> ret;

		if(auto deps = tags.getIf<Maike::KeyValueStore::ArrayRefConst>("dependencies"); deps)
		{
			std::transform(
			   std::begin(*deps), std::end(*deps), std::back_inserter(ret), [&src_dir](auto item) {
				   auto const val = item.template as<Maike::KeyValueStore::CompoundRefConst>();
				   return getDependency(src_dir, val, Maike::Db::SourceFileOrigin::Generated);
			   });
		}
		return ret;
	}

	Maike::Db::SourceFileInfo loadSourceFile(std::vector<Maike::Db::Dependency>&& builtin_deps,
	                                         Maike::fs::path const& path,
	                                         Maike::SourceFileInfoLoaders::Loader const& loader)
	{
		Maike::Io::Fifo<std::byte> src_fifo;
		Maike::Io::Fifo<std::byte> tags_fifo;

		Maike::Io::InputFile input{path};
		loader.filterInput(Maike::Io::Reader{input},
		                   Maike::SourceFileInfoLoaders::SourceOutStream{src_fifo},
		                   Maike::SourceFileInfoLoaders::TagsOutStream{tags_fifo});
		tags_fifo.stop();
		src_fifo.stop();

		{
			auto deps = fixNames(path.parent_path(), loader.getDependencies(Maike::Io::Reader{src_fifo}));
			builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
		}

		auto tags = Maike::KeyValueStore::Compound{Maike::Io::Reader{tags_fifo}, path.string()};
		auto targets = getTargets(path.parent_path(), tags);
		auto use_deps = getUseDeps(path.parent_path(), tags);
		auto child_target_use_deps = getChildTargetUseDeps(path.parent_path(), tags);

		std::copy(std::begin(use_deps), std::end(use_deps), std::back_inserter(builtin_deps));
		auto compiler = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("compiler");

		return Maike::Db::SourceFileInfo{std::move(builtin_deps),
		                                 std::vector<Maike::Db::Dependency>{},
		                                 std::move(child_target_use_deps),
		                                 std::move(targets),
		                                 compiler ? loader.getCompiler(*compiler) : loader.getCompiler()};
	}
}

std::optional<Maike::Db::SourceFileInfo>
Maike::SourceTreeLoader::SourceFileLoaderDelegator::load(Maike::fs::path const& path) const
{
	std::vector<Maike::Db::Dependency> deps;
	if(!path.parent_path().empty())
	{
		deps.push_back(Maike::Db::Dependency{path.parent_path().lexically_normal(),
		                                     Maike::Db::SourceFileOrigin::Project});
	}

	if(is_directory(path))
	{
		std::vector<Maike::Db::TargetInfo> targets;
		targets.push_back(Maike::Db::TargetInfo{path.lexically_normal(), std::vector<Db::Dependency>{}});
		return Maike::Db::SourceFileInfo{std::move(deps),
		                                 std::vector<Db::Dependency>{},
		                                 std::vector<Maike::Db::Dependency>{},
		                                 std::move(targets),
		                                 Maike::Compiler{MkDir{}}};
	}

	std::string extension;
	for(auto p = path; !p.extension().empty(); p = p.stem())
	{
		extension = std::string{p.extension()} + extension;
	}

	auto i = m_loaders.find(extension);
	if(i == std::end(m_loaders)) { return std::optional<Maike::Db::SourceFileInfo>{}; }

	return loadSourceFile(std::move(deps), path, i->second);
}
