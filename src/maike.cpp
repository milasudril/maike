//@	 {"targets":[{"name":"maike_next","type":"application"}, {"name":"maike.o","type":"object"}]}

#include "./cmd_line_options.hpp"
#include "./build_info.hpp"

#include "src/db/dependency_graph.hpp"
#include "src/io/input_file.hpp"
#include "src/config/main.hpp"
#include "src/source_tree_loader/directory_scanner.hpp"
#include "src/source_file_info_loaders/cxx/source_file_loader.hpp"
#include "src/utils/graphutils.hpp"
#include "src/utils/callwrappers.hpp"
#include "src/db/source_file_list.hpp"

void printDepGraph(Maike::Db::DependencyGraph const& source_files, Maike::fs::path const&)
{
	puts("digraph \"G\" {\nrankdir=LR\n");

	visitNodes(
	   [](auto const& item) {
		   auto id = item.id();
		   printf("\"%s (%zu)\"\n", item.path().c_str(), id.value());
		   visitEdges(
		      [&item, id](auto const& edge) {
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
		      },
		      item);
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
	void operator()(Duration duration, Maike::Db::SourceFileList const& result)
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
	void operator()(Duration duration, Maike::Db::TargetList const& result)
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

		auto const target_dir = cmdline.hasOption<Maike::CmdLineOption::TargetDir>() ?
		                           cmdline.option<Maike::CmdLineOption::TargetDir>() :
		                           Maike::fs::path{"__targets"};

		auto src_files = Maike::timedCall(logger,
		                                  Maike::SourceTreeLoader::load,
		                                  workers,
		                                  Maike::fs::path{"."},
		                                  cfg.sourceTreeLoader().inputFilter(),
		                                  loader_delegator,
		                                  target_dir);

		src_files.merge(Maike::timedCall(logger, Maike::Db::createDummySourceFiles, src_files));

		auto targets = Maike::timedCall(
		   logger,
		   [](auto&&... args) {
			   return Maike::Db::collectTargets(std::forward<decltype(args)>(args)...);
		   },
		   src_files);


		Maike::timedCall(
		   logger,
		   [](auto&&... args) {
			   makeSourceFileInfos(std::forward<decltype(args)>(args)...);
			   return Logger::SourceFilesFromTargets{};
		   },
		   targets,
		   src_files);

		auto graph = Maike::timedCall(
		   logger, [&src_files]() { return Maike::Db::DependencyGraph{std::move(src_files)}; });

		if(cmdline.hasOption<Maike::CmdLineOption::PrintDepGraph>())
		{
			printDepGraph(graph, cmdline.option<Maike::CmdLineOption::PrintDepGraph>());
			return 0;
		}

		Maike::visitNodesInTopoOrder(
		   [&graph](auto const& node) {
			   printf("%s  ", node.path().c_str());
			   if(isUpToDate(node, getUseDepsRecursive(graph, node))) { puts("Up-to-date"); }
			   else
			   {
				   puts("Compilation needed");
			   }
		   },
		   graph);

#if 0
		Maike::Db::visitNodes(
		   [&graph](Maike::Db::SourceFileRecordConst const& node) {
			   printf("%s\n", node.path().c_str());
#if 0
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
#endif
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
