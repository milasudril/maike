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
			   if(item.resolver() == Maike::Db::Dependency::Resolver::InternalLookup)
			   {
				   auto str = item.name().string();
				   if(str.size() > 1 && memcmp(str.data(), "./", 2) == 0)
				   {
					   return Maike::Db::Dependency{(prefix / item.name()).lexically_normal(), item.resolver()};
				   }
			   }
			   return item;
		   });
		return ret;
	}

	std::vector<Maike::fs::path> getTargets(Maike::fs::path const& src_dir,
	                                        Maike::KeyValueStore::Compound const& tags)
	{
		std::vector<Maike::fs::path> ret;
		if(auto target = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("target"); target)
		{
			auto const name = src_dir / target->template get<char const*>("name");
			ret.push_back(name);
		}

		if(auto targets = tags.getIf<Maike::KeyValueStore::ArrayRefConst>("targets"); targets)
		{
			std::transform(
			   std::begin(*targets), std::end(*targets), std::back_inserter(ret), [&src_dir](auto item) {
				   auto const val = item.template as<Maike::KeyValueStore::CompoundRefConst>();
				   return src_dir / Maike::fs::path{val.template get<char const*>("name")};
			   });
		}
		return ret;
	}

	std::vector<Maike::fs::path> getChildTargetUseDeps(Maike::fs::path const& src_dir,
	                                                   Maike::KeyValueStore::Compound const& tags)
	{
		std::vector<Maike::fs::path> ret;

		if(auto deps = tags.getIf<Maike::KeyValueStore::ArrayRefConst>("dependencies_extra"); deps)
		{
			std::transform(
			   std::begin(*deps), std::end(*deps), std::back_inserter(ret), [&src_dir](auto item) {
				   auto const val = item.template as<Maike::KeyValueStore::CompoundRefConst>();
				   return src_dir / Maike::fs::path{val.template get<char const*>("ref")};
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
		auto child_target_use_deps = getChildTargetUseDeps(path.parent_path(), tags);

		auto compiler = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("compiler");
		return Maike::Db::SourceFileInfo{std::move(builtin_deps),
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
		                                     Maike::Db::Dependency::Resolver::InternalLookup});
	}

	if(is_directory(path))
	{
		std::vector<Maike::fs::path> targets;
		targets.push_back(path.lexically_normal());
		return Maike::Db::SourceFileInfo{
		   std::move(deps), std::vector<Maike::fs::path>{}, std::move(targets), Maike::Compiler{MkDir{}}};
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
