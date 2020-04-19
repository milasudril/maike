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

Maike::SourceFileInfo loadSourceFile(Maike::fs::path const& path,
                                     Maike::Reader input,
                                     Maike::TagFilter filter,
                                     Maike::DependencyExtractor deps_extractor,
                                     Maike::fs::path const& target_dir)
{
	Maike::Fifo<std::byte> src_fifo;
	auto deps_fut =
	   std::async(std::launch::async, [src_reader = Maike::Reader{src_fifo}, &deps_extractor]() {
		   std::vector<Maike::Dependency> ret;
		   deps_extractor.run(src_reader, ret);
		   return ret;
	   });

	Maike::Fifo<std::byte> tags_fifo;
	auto tags_fut = std::async(
	   std::launch::async,
	   [tags_reader = Maike::Reader{tags_fifo}](std::string_view src_name) {
		   return Maike::KeyValueStore::Compound{tags_reader, src_name};
	   },
	   path.string());

	run(filter, input, Maike::SourceOutStream{src_fifo}, Maike::TagsOutStream{tags_fifo});
	tags_fifo.stop();
	src_fifo.stop();

	auto deps = [](Maike::fs::path const& src_dir, std::vector<Maike::Dependency>&& deps) {
		std::for_each(std::begin(deps), std::end(deps), [&src_dir](auto& item) {
			if(item.resolver() == Maike::Dependency::Resolver::InternalLookup
			   && item.name().string()[0] == '.')
			{ item = Maike::Dependency{(src_dir / item.name()).lexically_normal(), item.resolver()}; }
		});
		return deps;
	}(path.parent_path(), deps_fut.get());
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

	return Maike::SourceFileInfo{std::move(deps), target_dir, std::move(targets), Maike::Compiler{}};
}


std::optional<Maike::SourceFileInfo> loadSourceFile(Maike::fs::path const& path,
                                                    const Maike::fs::path& target_dir)
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
		return Maike::SourceFileInfo{
		   std::move(deps), target_dir, std::move(targets), Maike::Compiler{MkDir{}}};
	}

	auto extension = path.extension();
	if(extension == ".cpp" || extension == ".hpp")
	{
		Maike::InputFile input{path};
		Maike::Cxx::TagFilter filter;
		Maike::Cxx::SourceReader src_reader;
		return loadSourceFile(path,
		                      Maike::Reader{input},
		                      Maike::TagFilter{filter},
		                      Maike::DependencyExtractor{src_reader},
		                      target_dir);
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
	fflush(stdout);


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

		if(auto src_file_info = loadSourceFile(src_file_name_normal, target_dir);
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
			printf("\"%s (%p)\" -> \"%s\" (%p)\n",
			       node.name().c_str(),
			       node.fileInfo(),
			       edge.name().c_str(),
			       edge.sourceFile());
		});
	});
}
