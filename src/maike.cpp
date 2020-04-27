//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "./dependency_graph.hpp"

#include "./build_info.hpp"
#include "./stringutils.hpp"
#include "./local_system_invoker.hpp"
#include "./input_file.hpp"
#include "./fifo.hpp"
#include "./tag_filter.hpp"
#include "./reader.hpp"
#include "./writer.hpp"
#include "./dependency_extractor.hpp"
#include "./source_file_loader.hpp"
#include "key_value_store/compound.hpp"
#include "key_value_store/array.hpp"

#include "src/vcs_invoker/config.hpp"
#include "src/vcs_invoker/get_state_variables.hpp"

#include "src/cxx/source_file_loader.hpp"

#include <regex>
#include <future>

#include <unistd.h>

template<class T>
decltype(auto) pop(T& stack)
{
	auto ret = std::move(stack.top());
	stack.pop();
	return ret;
}

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
		auto deps = [](Maike::fs::path const& src_dir, std::vector<Maike::Dependency>&& deps) {
			std::for_each(std::begin(deps), std::end(deps), [&src_dir](auto& item) {
				if(item.resolver() == Maike::Dependency::Resolver::InternalLookup
				   && item.name().string()[0] == '.')
				{ item = Maike::Dependency{(src_dir / item.name()).lexically_normal(), item.resolver()}; }
			});
			return deps;
		}(path.parent_path(), deps_fut.get());

		builtin_deps.insert(std::end(builtin_deps), std::begin(deps), std::end(deps));
	};
	auto tags = tags_fut.get();

	auto targets = [](Maike::fs::path const& src_dir, Maike::KeyValueStore::Compound const& tags) {
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
	}(path.parent_path(), tags);

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

void visitChildren(Maike::SourceFile<Maike::ConstTag> src_file,
                   std::stack<Maike::fs::path>& paths_to_visit)
{
	if(is_directory(src_file.name()))
	{
		auto i = Maike::fs::directory_iterator{src_file.name()};
		std::for_each(
		   begin(i), end(i), [&paths_to_visit](auto const& item) { paths_to_visit.push(item.path()); });
	}
}

int main()
{
	// Config stuff
	std::vector<std::regex> input_filters;
	input_filters.push_back(std::regex{"/\\..*", std::regex_constants::basic});
	input_filters.push_back(std::regex{"/__*.*", std::regex_constants::basic});

	Maike::fs::path target_dir{"__targets_new"};
	Maike::VcsInvoker::Config vcs;

	//	Maike::LocalSystemInvoker invoker;

	// Current state
	Maike::DependencyGraph dep_graph;
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


	auto skip = [](auto const& path, auto const& regex_list) {
		return std::any_of(std::begin(regex_list), std::end(regex_list), [&path](auto const& regex) {
			return regex_search(path.c_str(), regex);
		});
	};

	// Loader
	std::stack<Maike::fs::path> paths_to_visit;
	paths_to_visit.push(Maike::fs::path{"."});
	while(!paths_to_visit.empty())
	{
		auto src_file_name = pop(paths_to_visit);
		if(skip(src_file_name, input_filters)) { continue; }

		auto src_file_name_normal = src_file_name.lexically_normal();
		if(dep_graph.find(src_file_name_normal) != nullptr) { continue; }

		if(auto src_file_info = loadSourceFile(src_file_name_normal);
		   src_file_info.has_value())
		{
			auto const& targets = src_file_info->targets();
			auto i = std::find_if(std::begin(targets), std::end(targets), [&dep_graph](auto const& item) {
				return dep_graph.find(item) != nullptr;
			});
			if(i != std::end(targets))
			{
				throw std::runtime_error{std::string{"Target "} + i->string() + " has already been defined."};
			}

			visitChildren(dep_graph.insert(std::move(src_file_name_normal), std::move(*src_file_info)),
			              paths_to_visit);
		}
	}

	dep_graph.visitItems([&dep_graph](auto node) {
		auto deps = node.usedFilesCopy();
		std::for_each(
		   std::begin(deps), std::end(deps), [&dep_graph](auto& edge) { edge.resolve(dep_graph); });
		node.usedFiles(std::move(deps));
	});


	dep_graph.visitItems([](auto node) {
		auto const& deps = node.usedFiles();
		std::for_each(std::begin(deps), std::end(deps), [&node](auto const& edge) {
			if(edge.sourceFile() != nullptr)
			{ printf("\"%s\" -> \"%s\"\n", node.name().c_str(), edge.name().c_str()); }
		});
	});
}
