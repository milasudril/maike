//@	 {"targets":[{"name":"maike_next","type":"application"}, {"name":"maike.o","type":"object"}]}

#include "./cmd_line_options.hpp"
#include "./build_info.hpp"

#include "src/db/target.hpp"
#include "src/db/dependency_graph.hpp"
#include "src/io/input_file.hpp"
#include "src/config/main.hpp"
#include "src/source_tree_loader/directory_scanner.hpp"
#include "src/source_file_info_loaders/cxx/source_file_loader.hpp"
#include "src/utils/graphutils.hpp"
#include "src/utils/callwrappers.hpp"

std::map<Maike::fs::path, Maike::Db::Target>
collectTargets(std::map<Maike::fs::path, Maike::Db::SourceFileInfo>& source_files)
{
	std::map<Maike::fs::path, Maike::Db::Target> ret;
	std::for_each(std::begin(source_files), std::end(source_files), [&ret](auto const& item) {
		auto const& targets = item.second.targets();
		std::for_each(std::begin(targets), std::end(targets), [&ret, &item](auto const& target) {
			if(item.first != target.name()) // For backwards compatiblity with old maike
			{
				auto i = ret.find(target.name());
				if(i != std::end(ret)) { throw std::runtime_error{"Target has already been defined"}; }

				ret.insert(std::make_pair(target.name(), Maike::Db::Target{item.first, item.second}));
			}
		});
	});
	return ret;
}

void makeSourceFileInfosFromTargets(
   const std::map<Maike::fs::path, Maike::Db::Target>& targets,
   std::map<Maike::fs::path, Maike::Db::SourceFileInfo>& source_files,
   Maike::fs::path const& target_dir)
{
	Maike::Db::DependencyGraph g{std::move(source_files)};
	source_files = g.takeSourceFiles();
	std::for_each(
	   std::begin(targets), std::end(targets), [&g, &source_files, &target_dir](auto const& item) {
		   std::vector<Maike::Db::Dependency> use_deps;
		   auto node = getNode(g, item.second.sourceFilename());
		   Maike::processGraphNodeRecursive(
		      [&use_deps, &target_dir, &target_name = item.first](auto const& node) {
			      auto const& child_target_use_deps = node.sourceFileInfo().childTargetsUseDeps();
			      std::for_each(std::begin(child_target_use_deps),
			                    std::end(child_target_use_deps),
			                    [&use_deps, &target_dir, &target_name](auto const& item) {
				                    if(item != target_name) // A target may never point to itself
				                    {
					                    // FIXME: These deps need resolver info (in case it was a pkgconfig lookup)
					                    use_deps.push_back(Maike::Db::Dependency{
					                       target_dir / item, Maike::Db::Dependency::Resolver::InternalLookup});
				                    }
			                    });
		      },
		      g,
		      node);

		   Maike::Db::SourceFileInfo src_file;

		   // FIXME: Remove duplicates before sinking into src_file

		   src_file.useDeps(std::move(use_deps));
		   source_files.insert(std::make_pair(target_dir / item.first, std::move(src_file)));
	   });
}


void printDepGraph(Maike::Db::DependencyGraph const& source_files, Maike::fs::path const&)
{
	puts("digraph \"G\" {\nrankdir=LR\n");

	visitNodes(
	   [](auto const& item) {
		   auto const& deps = item.sourceFileInfo().useDeps();
		   auto id = item.id();
		   printf("\"%s (%zu)\"\n", item.path().c_str(), id.value());
		   std::for_each(std::begin(deps), std::end(deps), [&item, id](auto const& edge) {
			   if(edge.reference().valid())
			   {
				   printf("\"%s (%zu)\" -> \"%s (%zu)\"\n",
				          item.path().c_str(),
				          id.value(),
				          edge.name().c_str(),
				          edge.reference().value());
			   }
			   else
			   {
				   printf("\"%s (%zu)\" -> \"%s (unresolved)\"\n",
				          item.path().c_str(),
				          id.value(),
				          edge.name().c_str());
			   }
		   });
	   },
	   source_files);
	puts("}");
}

void printHelp(Maike::CommandLine const& cmdline)
{
	auto info = cmdline.optionInfo();

	std::for_each(std::begin(info),
	              std::end(info),
	              [category = static_cast<char const*>(nullptr)](auto const& item) mutable {
		              if(category == nullptr || strcmp(category, item.category()) != 0)
		              {
			              printf("\n## %s\n\n", item.category());
			              category = item.category();
		              }
		              if(item.type() == nullptr)
		              {
			              printf("%s\n    %s\n\n", item.name(), item.summary());
			              return;
		              }

		              if(item.valueRequired())
		              {
			              printf("%s=`%s`\n    %s\n\n", item.name(), item.type(), item.summary());
			              return;
		              }
		              printf("%s[=`%s`]\n    %s\n\n", item.name(), item.type(), item.summary());
		              return;
	              });
}


void print(char const* name, Maike::fs::path const& path)
{
	fprintf(stderr, "%s: %s", name, path.c_str());
}

void print(char const* name, Maike::BuildId const& id)
{
	fprintf(stderr, "%s: %s", name, toString(id).c_str());
}

void print(char const* name, Maike::ThreadCount count)
{
	fprintf(stderr, "%s: %s", name, toString(count).c_str());
}

void print(char const* name, std::false_type)
{
	fprintf(stderr, "%s: %s", name, "");
}

void print(char const* name, Maike::SystemTimeStamp t)
{
	auto timeval = std::chrono::system_clock::to_time_t(t.value());

	tm time_decomposed;
	gmtime_r(&timeval, &time_decomposed);

	fprintf(stderr,
	        "%s: %04d-%02d-%02d %d:%02d:%02d UTC",
	        name,
	        time_decomposed.tm_year + 1900,
	        time_decomposed.tm_mon + 1,
	        time_decomposed.tm_mday,
	        time_decomposed.tm_hour,
	        time_decomposed.tm_min,
	        time_decomposed.tm_sec);
}


template<class T>
void print(char const* name, std::vector<T> const& v)
{
	fprintf(stderr, "%s: ", name);
	std::for_each(std::begin(v), std::end(v), [](auto const& val) {
		print(nullptr, val);
		fprintf(stderr, ", ");
	});
}

void write(FILE* stream, void const* buffer, size_t n)
{
	fwrite(buffer, 1, n, stream);
}

void dumpConfig(Maike::Config::Main const& cfg, Maike::fs::path const&)
{
	store(Maike::KeyValueStore::Compound{}.set("maikeconfig", cfg).handleReference(), stdout);
}

class Logger
{
public:
	struct SourceFilesFromTargets
	{
	};

	template<class Duration>
	void operator()(Duration duration,
	                std::map<Maike::fs::path, Maike::Db::SourceFileInfo> const& result)
	{
		fprintf(stderr,
		        "# Processed %zu files in %.7f seconds (%.7e seconds/file)\n",
		        result.size(),
		        std::chrono::duration<double>(duration).count(),
		        std::chrono::duration<double>(duration).count() / result.size());
	}

	template<class Duration>
	void operator()(Duration duration, Maike::Db::DependencyGraph const& result)
	{
		fprintf(stderr,
		        "# Create dependency graph with %zu entries in %.7f seconds  (%.7e seconds/entry)\n",
		        size(result),
		        std::chrono::duration<double>(duration).count(),
		        std::chrono::duration<double>(duration).count() / size(result));
	}

	template<class Duration>
	void operator()(Duration duration, SourceFilesFromTargets)
	{
		fprintf(stderr,
		        "# Generated source file entries from collected targets in %.7f seconds\n",
		        std::chrono::duration<double>(duration).count());
	}

	template<class Duration>
	void operator()(Duration duration, std::map<Maike::fs::path, Maike::Db::Target> const& result)
	{
		fprintf(stderr,
		        "# Collected %zu targets in %.7f seconds (%.7e seconds/entry)\n",
		        result.size(),
		        std::chrono::duration<double>(duration).count(),
		        std::chrono::duration<double>(duration).count() / result.size());
	}
};

int main(int argc, char** argv)
{
	try
	{
		Maike::CommandLine cmdline{argc - 1, argv + 1};
		if(cmdline.hasOption<Maike::CmdLineOption::Help>())
		{
			printHelp(cmdline);
			return 0;
		}

		cmdline.forEachOption([](char const* name, auto const& val) {
			print(name, val);
			putchar('\n');
		});


		/*		{
		   printf(">>> %s\n", toString(cmdline.option<Maike::CmdLineOption::BuildId>()).c_str());
		   return 0;
		  }*/

		Maike::KeyValueStore::init();

		auto cfg = Maike::Config::load(cmdline.hasOption<Maike::CmdLineOption::ConfigFiles>() ?
		                                  cmdline.option<Maike::CmdLineOption::ConfigFiles>() :
		                                  std::vector<Maike::fs::path>{"maikeconfig.json"});


		if(cmdline.hasOption<Maike::CmdLineOption::ConfigDump>())
		{
			dumpConfig(cfg, cmdline.option<Maike::CmdLineOption::ConfigDump>());
			return 0;
		}


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

		Logger logger;
		Maike::Sched::ThreadPool workers{cmdline.option<Maike::CmdLineOption::NumWorkers>()};

		Maike::SourceTreeLoader::SourceFileLoaderDelegator loader_delegator;
		loader_delegator.loaders(mapSourceFileInfoLoaders(cfg));

		auto src_files = Maike::timedCall(logger,
		                                  Maike::SourceTreeLoader::load,
		                                  workers,
		                                  Maike::fs::path{"."},
		                                  cfg.sourceTreeLoader().inputFilter(),
		                                  loader_delegator);

		auto targets = Maike::timedCall(logger, collectTargets, src_files);


		auto const target_dir = cmdline.hasOption<Maike::CmdLineOption::TargetDir>() ?
		                           cmdline.option<Maike::CmdLineOption::TargetDir>() :
		                           Maike::fs::path{"__targets"};

		Maike::timedCall(
		   logger,
		   [](auto&&... args) {
			   makeSourceFileInfosFromTargets(std::forward<decltype(args)>(args)...);
			   return Logger::SourceFilesFromTargets{};
		   },
		   targets,
		   src_files,
		   target_dir);

		auto graph = Maike::timedCall(
		   logger, [&src_files]() { return Maike::Db::DependencyGraph{std::move(src_files)}; });

		if(cmdline.hasOption<Maike::CmdLineOption::PrintDepGraph>())
		{
			printDepGraph(graph, cmdline.option<Maike::CmdLineOption::PrintDepGraph>());
			return 0;
		}


#if 0
		Maike::Db::visitNodes(
		   [&graph](Maike::Db::SourceFileRecordConst const& node) {
			   printf("%s\n", node.path().c_str());
			   Maike::processGraphNodeRecursive(
			      [](auto const& edge) {
				      printf("    %s (%zu)\n", edge.path().c_str(), edge.id().value());
				      auto const& target_use_deps = edge.sourceFileInfo().childTargetsUseDeps();
				      std::for_each(std::begin(target_use_deps),
				                    std::end(target_use_deps),
				                    [](auto const& name) { printf("        %s\n", name.c_str()); });
			      },
			      graph,
			      node);
		   },
		   graph);
#endif
	}
	catch(std::exception const& err)
	{
		fprintf(stderr, "%s\n", err.what());
		return -1;
	}
}
