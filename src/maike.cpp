//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "./build_info.hpp"
#include "./stringutils.hpp"
#include "./local_system_invoker.hpp"
#include "./input_file.hpp"
#include "./fifo.hpp"
#include "./tag_filter.hpp"
#include "./reader.hpp"
#include "./writer.hpp"
#include "./source_file_loader.hpp"
#include "./config.hpp"
#include "key_value_store/compound.hpp"
#include "key_value_store/array.hpp"

#include "src/cxx/source_file_loader.hpp"

#include <future>

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

	Maike::KeyValueStore::Compound settings() const
	{
		return Maike::KeyValueStore::Compound{};
	}

	void settings(Maike::KeyValueStore::Compound&&)
	{
	}
};

std::vector<Maike::Dependency> fixNames(Maike::fs::path const& prefix,
                                        std::vector<Maike::Dependency> const& deps)
{
	std::vector<Maike::Dependency> ret;
	ret.reserve(deps.size());
	std::transform(
	   std::begin(deps), std::end(deps), std::back_inserter(ret), [&prefix](auto const& item) {
		   if(item.resolver() == Maike::Dependency::Resolver::InternalLookup)
		   {
			   auto const& str = item.name().string();
			   if(str.size() > 1 && memcmp(str.data(), "./", 2) == 0)
			   { return Maike::Dependency{(prefix / item.name()).lexically_normal(), item.resolver()}; }
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
		auto const name = target->template get<char const*>("name");
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

Maike::SourceFileInfo loadSourceFile(std::vector<Maike::Dependency>&& builtin_deps,
                                     Maike::fs::path const& path,
                                     Maike::SourceFileLoader const& loader)
{
	Maike::Fifo<std::byte> src_fifo;
	auto deps_fut = std::async(std::launch::async, [&loader, input = Maike::Reader{src_fifo}]() {
		return loader.getDependencies(input);
	});

	Maike::Fifo<std::byte> tags_fifo;
	auto tags_fut = std::async(
	   std::launch::async,
	   [tags_reader = Maike::Reader{tags_fifo}](std::string_view src_name) {
		   return Maike::KeyValueStore::Compound{tags_reader, src_name};
	   },
	   path.string());

	Maike::InputFile input{path};
	loader.filterInput(
	   Maike::Reader{input}, Maike::SourceOutStream{src_fifo}, Maike::TagsOutStream{tags_fifo});
	tags_fifo.stop();
	src_fifo.stop();

	{
		auto deps = fixNames(path.parent_path(), deps_fut.get());
		builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
	}

	auto tags = tags_fut.get();
	auto targets = getTargets(path.parent_path(), tags);

	auto compiler = tags.getIf<Maike::KeyValueStore::CompoundRefConst>("compiler");
	return Maike::SourceFileInfo{std::move(builtin_deps),
	                             std::move(targets),
	                             compiler ? loader.getCompiler(*compiler) : loader.getCompiler()};
}


std::optional<Maike::SourceFileInfo> loadSourceFile(Maike::fs::path const& path)
{
	std::vector<Maike::Dependency> deps;
	if(!path.parent_path().empty())
	{
		deps.push_back(Maike::Dependency{path.parent_path().lexically_normal(),
		                                 Maike::Dependency::Resolver::InternalLookup});
	}

	if(is_directory(path))
	{
		std::vector<Maike::fs::path> targets;
		targets.push_back(path.lexically_normal());
		return Maike::SourceFileInfo{std::move(deps), std::move(targets), Maike::Compiler{MkDir{}}};
	}

	auto extension = path.extension();
	if(extension == ".cpp" || extension == ".hpp")
	{
		return loadSourceFile(
		   std::move(deps), path, Maike::SourceFileLoader{Maike::Cxx::SourceFileLoader{}});
	}

	return std::optional<Maike::SourceFileInfo>{};
}

void visitChildren(Maike::fs::path const& src_file, std::stack<Maike::fs::path>& paths_to_visit)
{
	if(is_directory(src_file))
	{
		auto i = Maike::fs::directory_iterator{src_file};
		std::for_each(
		   begin(i), end(i), [&paths_to_visit](auto const& item) { paths_to_visit.push(item.path()); });
	}
}

template<class T>
decltype(auto) pop(T& stack)
{
	auto ret = std::move(stack.top());
	stack.pop();
	return ret;
}

std::map<Maike::fs::path, Maike::SourceFileInfo>
loadSourceFiles(Maike::InputFilter const& filter,
                Maike::fs::path const& start_dir = Maike::fs::path{"."})
{
	std::map<Maike::fs::path, Maike::SourceFileInfo> ret;
	std::stack<Maike::fs::path> paths_to_visit;
	paths_to_visit.push(start_dir);

	while(!paths_to_visit.empty())
	{
		auto src_path = pop(paths_to_visit);
		if(src_path != "." && filter.match(src_path.filename().c_str())) { continue; }

		auto src_path_normal = src_path.lexically_normal();
		auto i = ret.find(src_path_normal);
		if(i != std::end(ret)) { continue; }

		if(auto src_file_info = loadSourceFile(src_path_normal); src_file_info.has_value())
		{
			auto ins = ret.insert(i, std::make_pair(std::move(src_path_normal), std::move(*src_file_info)));
			visitChildren(ins->first, paths_to_visit);
		}
	}
	return ret;
}

void makeSourceFileInfosFromTargets(std::map<Maike::fs::path, Maike::SourceFileInfo>& source_files)
{
	std::for_each(std::begin(source_files), std::end(source_files), [&source_files](auto const& item) {
		auto const& targets = item.second.targets();
		std::for_each(std::begin(targets), std::end(targets), [&source_files, &item](auto const& target) {
			if(item.first != target) // For backwards compatiblity with old maike
			{
				auto i = source_files.find(target);
				if(i != std::end(source_files)) { throw std::runtime_error{"Target has already been defined"}; }

				std::vector<Maike::Dependency> deps{
				   Maike::Dependency{item.first, Maike::Dependency::Resolver::InternalLookup}};
				Maike::SourceFileInfo src_file;
				src_file.usedFiles(std::move(deps));
				// TODO: Should target dir be prepended to target?
				source_files.insert(i, std::make_pair(target, std::move(src_file)));
			}
		});
	});
}

void resolveDependencies(std::map<Maike::fs::path, Maike::SourceFileInfo>& source_files)
{
	std::for_each(std::begin(source_files), std::end(source_files), [&source_files](auto& item) {
		auto const& deps = item.second.usedFiles();
		std::vector<Maike::Dependency> deps_resolved;
		deps_resolved.reserve(deps.size());
		std::transform(std::begin(deps),
		               std::end(deps),
		               std::back_inserter(deps_resolved),
		               [&source_files](auto const& edge) {
			               auto tmp = edge;
			               return tmp.resolve(source_files);
		               });
		item.second.usedFiles(std::move(deps_resolved));
	});
}


int main()
{
	Maike::Config cfg;


	//	Maike::LocalSystemInvoker invoker;

	// Current state

	/*	Maike::BuildInfo bi{Maike::VcsState{getStateVariables(std::cref(vcs), invoker)}};

	 store(toJson(bi), BuildInfoSink{});

	 printf(
	    "\n\n#       Start time: %s\n"
	    "#               Id: %s\n"
	    "#     VCS revision: %s\n"
	    "#  VCS version tag: %s\n"
	    "#       VCS branch: %s\n",
	    Maike::toString(bi.startTime()).c_str(),
	    toString(bi.buildId()).c_str(),
	    bi.vcsState().revision().c_str(),
	    bi.vcsState().versionTag().c_str(),
	    bi.vcsState().branch().c_str());
	 fflush(stdout);*/

	auto src_files = loadSourceFiles(cfg.inputFilter());
	makeSourceFileInfosFromTargets(src_files);
	resolveDependencies(src_files);

	std::for_each(std::begin(src_files), std::end(src_files), [](auto const& item) {
		auto const& deps = item.second.usedFiles();
		printf("\"%s\"\n", item.first.c_str());
		std::for_each(std::begin(deps), std::end(deps), [&item](auto const& edge) {
			if(edge.sourceFile() != nullptr)
			{ printf("\"%s\" -> \"%s\"\n", item.first.c_str(), edge.name().c_str()); }
		});
	});
}
