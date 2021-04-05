//@	{
//@	 "targets":[{"name":"source_file_loader_delegator.o","type":"object"}]
//@	}

#include "./source_file_loader_delegator.hpp"

#include "./source_file_load_ctxt.hpp"

#include "src/io/fifo.hpp"
#include "src/io/input_file.hpp"
#include "src/io/reader.hpp"

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

	std::vector<Maike::Db::Dependency>
	addPrefixWhereAppiciable(Maike::fs::path const& prefix,
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

	Maike::Db::SourceFileInfo loadSourceFile(std::vector<Maike::Db::Dependency>&& builtin_deps,
	                                         Maike::fs::path const& path,
	                                         Maike::SourceFileInfoLoaders::Loader const& loader,
	                                         Maike::fs::path const& target_dir)
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
			auto deps = addPrefixWhereAppiciable(path.parent_path(),
			                                     loader.getDependencies(Maike::Io::Reader{src_fifo}));
			builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
		}

		auto tags = Maike::KeyValueStore::Compound{Maike::Io::Reader{tags_fifo}, path.string()};
		Maike::SourceTreeLoader::SourceFileLoadContext load_ctxt{path.parent_path(), target_dir};
		auto targets = getTargets(load_ctxt, tags);
		auto use_deps = getUseDeps(load_ctxt, tags);
		auto child_target_use_deps = getChildTargetUseDeps(load_ctxt, tags);

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
Maike::SourceTreeLoader::SourceFileLoaderDelegator::load(fs::path const& path,
                                                         fs::path const& target_dir) const
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
		targets.push_back(
		   Maike::Db::TargetInfo{target_dir / (path.lexically_normal()), std::vector<Db::Dependency>{}});
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

	return loadSourceFile(std::move(deps), path, i->second, target_dir);
}
