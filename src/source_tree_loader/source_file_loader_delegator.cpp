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

	Maike::Db::SourceFileInfo loadSourceFile(Maike::fs::path const& src_path,
	                                         Maike::fs::path const& target_dir,
	                                         std::vector<Maike::Db::Dependency>&& builtin_deps,
	                                         Maike::SourceFileInfoLoaders::Loader const& loader)
	{
		Maike::Io::Fifo<std::byte> src_fifo;
		Maike::Io::Fifo<std::byte> tags_fifo;

		Maike::Io::InputFile input{src_path};
		loader.filterInput(Maike::Io::Reader{input},
		                   Maike::SourceFileInfoLoaders::SourceOutStream{src_fifo},
		                   Maike::SourceFileInfoLoaders::TagsOutStream{tags_fifo});
		tags_fifo.stop();
		src_fifo.stop();

		{
			auto deps = addPrefixWhereAppiciable(src_path.parent_path(),
			                                     loader.getDependencies(Maike::Io::Reader{src_fifo}));
			builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
		}

		auto tags = Maike::KeyValueStore::Compound{Maike::Io::Reader{tags_fifo}, src_path.string()};
		Maike::SourceTreeLoader::SourceFileLoadContext load_ctxt{src_path.parent_path(), target_dir};
		auto targets = getTargets(load_ctxt, tags);
		auto use_deps = getUseDeps(load_ctxt, tags);
		auto child_target_use_deps = getChildTargetUseDeps(load_ctxt, tags);

		std::copy(std::begin(use_deps), std::end(use_deps), std::back_inserter(builtin_deps));
		auto compiler = tags.getIf<Maike::Db::Compiler>("compiler");

		return Maike::Db::SourceFileInfo{std::move(builtin_deps),
		                                 std::vector<Maike::Db::Dependency>{},
		                                 std::move(child_target_use_deps),
		                                 std::move(targets),
		                                 std::ref(loader.compiler()),
		                                 compiler ? (*compiler) : Maike::Db::Compiler{""},
		                                 Maike::Db::SourceFileOrigin::Project};
	}
}

static const Maike::Db::Compiler mkdir{"make_directory"};

std::optional<Maike::Db::SourceFileInfo>
Maike::SourceTreeLoader::SourceFileLoaderDelegator::load(fs::path const& str_path,
                                                         fs::path const& target_dir) const
{
	std::vector<Db::Dependency> deps;
	if(!str_path.parent_path().empty())
	{
		deps.push_back(
		   Db::Dependency{str_path.parent_path().lexically_normal(), Db::SourceFileOrigin::Project});
	}

	if(is_directory(str_path))
	{
		std::vector<Db::TargetInfo> targets;
		targets.push_back(
		   Db::TargetInfo{target_dir / (str_path.lexically_normal()), std::vector<Db::Dependency>{}});
		return Db::SourceFileInfo{std::move(deps),
		                          std::vector<Db::Dependency>{},
		                          std::vector<Db::Dependency>{},
		                          std::move(targets),
		                          mkdir,
		                          Db::Compiler{""},
		                          Db::SourceFileOrigin::Project};
	}

	std::string extension;
	for(auto p = str_path; !p.extension().empty(); p = p.stem())
	{
		extension = std::string{p.extension()} + extension;
	}

	auto i = m_loaders.find(extension);
	if(i == std::end(m_loaders)) { return std::optional<Db::SourceFileInfo>{}; }

	return loadSourceFile(str_path, target_dir, std::move(deps), i->second);
}
