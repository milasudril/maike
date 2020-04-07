//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "./dependency_graph.hpp"

#include "./build_info.hpp"
#include "./stringutils.hpp"
#include "./local_system_invoker.hpp"
#include "./input_file.hpp"
#include "./fifo.hpp"

#include "src/vcs_invoker/config.hpp"
#include "src/vcs_invoker/get_state_variables.hpp"

#include "src/cxx/tag_filter.hpp"
#include "src/cxx/source_reader.hpp"

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

	Maike::KeyValueStore::JsonRefConst settings() const
	{
		return m_settings.reference();
	}

	void settings(Maike::KeyValueStore::Compound&&)
	{
	}

private:
	Maike::KeyValueStore::Compound m_settings;
};

struct TestSink
{
};

void write(TestSink&, char const* buffer, size_t n)
{
	write(1, buffer, n);
}

std::optional<Maike::SourceFileInfo> loadSourceFile(Maike::fs::path const& path,
                                                    const Maike::fs::path& target_dir)
{
	std::vector<Maike::Dependency> deps;
	if(!path.parent_path().empty())
	{
		deps.push_back(
		   Maike::Dependency{path.parent_path(), Maike::Dependency::Resolver::InternalLookup});
	}
	std::vector<Maike::fs::path> targets;

	if(is_directory(path))
	{
		targets.push_back(path);
		return Maike::SourceFileInfo{
		   std::move(deps), target_dir, std::move(targets), Maike::Compiler{MkDir{}}};
	}

	auto extension = path.extension();
	printf("%s\n", path.c_str());
	if(extension == ".cpp" || extension == ".hpp")
	{
		//	TODO:
		Maike::InputFile input{path};
		Maike::Fifo<std::byte> tag_fifo;
		Maike::Fifo<std::byte> src_fifo;
		auto deps_from_src = std::async(
		   std::launch::async,
		   [](Maike::Fifo<std::byte>& src) {
			   Maike::Cxx::SourceReader src_reader;
			   std::vector<Maike::Dependency> ret;
			   src_reader.run(Maike::Reader{src}, ret);
			   return ret;
		   },
		   std::ref(src_fifo));

		auto tags_fut = std::async(
		   std::launch::async,
		   [](Maike::Fifo<std::byte>& src, Maike::fs::path const& filename) {
			   auto const filename_string = filename.string();
			   return Maike::KeyValueStore::Compound{src, filename_string};
		   },
		   std::ref(tag_fifo),
		   path);

		Maike::Cxx::TagFilter filter;
		filter.run(Maike::Reader{input}, Maike::Writer{tag_fifo}, Maike::Writer{src_fifo});
		src_fifo.stop();
		auto deps = deps_from_src.get();
		std::for_each(std::begin(deps), std::end(deps), [](auto const& item) {
			printf("%s ", item.name().c_str());
		});

		tag_fifo.stop();
		auto tags = tags_fut.get();
		store(tags, TestSink{});
		putchar('\n');
		putchar('\n');
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

namespace
{
	struct BuildInfoSink
	{
	};

	void write(BuildInfoSink, char const* buffer, size_t bufflen)
	{
		::write(1, buffer, bufflen);
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

	Maike::LocalSystemInvoker invoker;

	// Current state
	Maike::DependencyGraph dep_graph;
	Maike::BuildInfo bi{Maike::VcsState{getStateVariables(std::cref(vcs), invoker)}};

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

		if(dep_graph.find(src_file_name) != nullptr) { continue; }

		if(auto src_file_info = loadSourceFile(src_file_name, target_dir); src_file_info.has_value())
		{
			auto const& targets = src_file_info->targets();
			if(std::any_of(std::begin(targets), std::end(targets), [&dep_graph](auto const& item) {
				   return dep_graph.find(item) != nullptr;
			   }))
			{ throw std::runtime_error{"Target has already been defined"}; }

			visitChildren(dep_graph.insert(std::move(src_file_name), std::move(*src_file_info)),
			              paths_to_visit);
		}
	}

	dep_graph.visitItems([&dep_graph](auto node) {
		auto deps = node.usedFilesCopy();
		std::for_each(
		   std::begin(deps), std::end(deps), [&dep_graph](auto& edge) { edge.resolve(dep_graph); });
		node.usedFiles(std::move(deps));
	});

	/*
	dep_graph.visitItems([](auto node) {
	 auto const& deps = node.usedFiles();
	 std::for_each(std::begin(deps), std::end(deps), [&node](auto const& edge) {
	  printf("\"%s (%p)\" -> \"%s\" (%p)\n",
	         node.name().c_str(),
	         node.fileInfo(),
	         edge.name().c_str(),
	         edge.sourceFile());
	 });
	});*/
}