//@	 {"targets":[{"name":"maike_next","type":"application"}, {"name":"maike.o","type":"object"}]}

#include "./cmd_line_options.hpp"
#include "./build_info.hpp"

#include "src/db/dependency_graph.hpp"
#include "src/io/input_file.hpp"
#include "src/config/main.hpp"
#include "src/source_tree_loader/main.hpp"
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
	template<class Duration>
	void operator()(Duration duration, Maike::Db::SourceTree const& result)
	{
		auto const n = size(result.dependencyGraph());
		fprintf(
		   stderr,
		   "# Loaded source tree with %zu entries (%zu targets) in %.7f seconds  (%.7e seconds/entry)\n",
		   n,
		   size(result.targets()),
		   std::chrono::duration<double>(duration).count(),
		   std::chrono::duration<double>(duration).count() / n);
	}

	template<class Duration>
	void operator()(Duration duration, Maike::Db::TaskCounter n)
	{
		fprintf(stderr,
		        "# Completed %zu tasks in %.7f seconds (%.7e seconds/task)\n",
		        n.value(),
		        std::chrono::duration<double>(duration).count(),
		        std::chrono::duration<double>(duration).count() / n.value());
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

		auto const src_tree = Maike::timedCall(logger,
		                                       Maike::SourceTreeLoader::load,
		                                       workers,
		                                       Maike::fs::path{"."},
		                                       cfg.sourceTreeLoader().inputFilter(),
		                                       loader_delegator,
		                                       target_dir);

		if(cmdline.hasOption<Maike::CmdLineOption::PrintDepGraph>())
		{
			printDepGraph(src_tree.dependencyGraph(), cmdline.option<Maike::CmdLineOption::PrintDepGraph>());
			return 0;
		}

		Maike::timedCall(
		   logger,
		   [](auto&&... args) { return compile(std::forward<decltype(args)>(args)...); },
		   src_tree,
		   Maike::Db::ForceRecompilation{},
		   workers);
	}
	catch(std::exception const& err)
	{
		fprintf(stderr, "%s\n", err.what());
		return -1;
	}
}
