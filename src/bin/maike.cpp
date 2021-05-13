//@	{
//@	 "target":{"name":"maike.o","type":"object"}
//@	}

#include "./cmd_line_options.hpp"
#include "./config.hpp"
#include "./about.hpp"

#include "src/build/info.hpp"
#include "src/db/dependency_graph.hpp"
#include "src/io/input_file.hpp"
#include "src/source_tree_loader/main.hpp"
#include "src/source_tree_loader/command_dictionary.hpp"
#include "src/source_file_info_loaders/cxx/source_file_loader.hpp"
#include "src/utils/graphutils.hpp"
#include "src/utils/callwrappers.hpp"
#include "src/db/source_file_list.hpp"
#include "src/db/compilation_log.hpp"
#include "src/exec/execve.hpp"

#include <set>

Maike::Build::Info makeBuildInfo(Maike::CommandLine const& cmdline)
{
	Maike::Build::Info ret;

	if(cmdline.hasOption<Maike::CmdLineOption::StartTime>())
	{ ret.startTime(cmdline.option<Maike::CmdLineOption::StartTime>()); }

	if(cmdline.hasOption<Maike::CmdLineOption::BuildId>())
	{ ret.buildId(Maike::Build::Id{cmdline.option<Maike::CmdLineOption::BuildId>()}); }

	if(cmdline.hasOption<Maike::CmdLineOption::SourceDir>())
	{ ret.sourceDir(Maike::fs::path{cmdline.option<Maike::CmdLineOption::SourceDir>()}); }

	if(cmdline.hasOption<Maike::CmdLineOption::TargetDir>())
	{ ret.targetDir(Maike::fs::path{cmdline.option<Maike::CmdLineOption::TargetDir>()}); }
	else
	{
		if(ret.sourceDir().filename() != ".")
		{ ret.targetDir(Maike::fs::path{ret.sourceDir().filename().string() + "_targets"}); }
	}

	return ret;
}

Maike::Db::CompilationLog::OutputFormat getOutputFormat(Maike::CommandLine const& cmdline)
{
	if(cmdline.hasOption<Maike::CmdLineOption::LogOutputFormat>())
	{ return cmdline.option<Maike::CmdLineOption::LogOutputFormat>(); }

	return isatty(STDERR_FILENO) ? Maike::Db::CompilationLog::OutputFormat::AnsiTerm :
	                               Maike::Db::CompilationLog::OutputFormat::PlainText;
}

Maike::Db::CompilationLog::LogLevel getLogLevel(Maike::CommandLine const& cmdline)
{
	return cmdline.hasOption<Maike::CmdLineOption::LogLevel>() ?
	          cmdline.option<Maike::CmdLineOption::LogLevel>() :
	          Maike::Db::CompilationLog::LogLevel::SourceFileInfo;
}

void printDepGraph(Maike::Db::DependencyGraph const& source_files, Maike::fs::path const&)
{
	std::set<std::pair<Maike::fs::path, Maike::fs::path>> output;
	visitNodes(
	   [&output](auto const& item) {
		   visitEdges(
		      [&output, &item](auto const& edge) {
#if 0
			      if(item.path().parent_path() != edge.name().parent_path())
			      { output.insert(std::pair{item.path().parent_path(), edge.name().parent_path()}); }
#else
			      output.insert(std::pair{item.path(), edge.name()});
#endif
		      },
		      item);
	   },
	   source_files);
	puts("digraph \"G\" {\nrankdir=LR\n");
	std::for_each(std::begin(output), std::end(output), [](auto const& item) {
		printf("\"%s\" -> \"%s\"\n", item.first.c_str(), item.second.c_str());
	});
	puts("}");
}

void printHelp(Maike::CommandLine const& cmdline)
{
	printf(
	   "\nBelow is a summary of all command line options. For information about how to type "
	   "arguments, see *Parameter type explanations* below.\n");

	constexpr auto info = cmdline.optionInfo();

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
			              printf("%s=*%s*\n    %s\n\n", item.name(), item.type(), item.summary());
			              return;
		              }
		              printf("%s[=*%s*]\n    %s\n\n", item.name(), item.type(), item.summary());
		              return;
	              });

	constexpr auto typeinfo = cmdline.typeInfo();
	if(std::size(typeinfo) != 0)
	{
		printf(
		   "\n___\n\n"
		   "## Parameter type explanations\n\n"
		   "This section contains summaries of different parameter types.\n\n");

		std::for_each(std::begin(typeinfo), std::end(typeinfo), [](auto const& item) {
			if(item.name() != nullptr) { printf("\n### %s\n\n%s\n\n", item.name(), item.description()); }
		});
	}
}

void printVersion(Maike::fs::path const&)
{
	printf(R"STR(# Maike %s (%s)

Build start time: %s

Build id: %s

Build host: %s

)STR",
	       Maike::Self::VcsTag,
	       Maike::Self::VcsRevisionId,
	       Maike::Self::BuildStartTime,
	       Maike::Self::BuildId,
	       Maike::Self::BuildHost);
}

void printAbout(Maike::fs::path const&)
{
	printf(R"STR(# Maike %s (%s)

%s

%s

%s

## Build info

Start time: %s

Id: %s

Build host: %s

)STR",
	       Maike::Self::VcsTag,
	       Maike::Self::VcsRevisionId,
	       Maike::Self::DescriptionShort,
	       Maike::Self::Copyright,
	       Maike::Self::LegalBrief,
	       Maike::Self::BuildStartTime,
	       Maike::Self::BuildId,
	       Maike::Self::BuildHost);
}

void print(char const* name, Maike::fs::path const& path)
{
	fprintf(stderr, "%s: %s", name, path.c_str());
}

void print(char const* name, Maike::Build::Id const& id)
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

void print(char const* name, Maike::Db::CompilationLog::LogLevel level)
{
	fprintf(stderr, "%s: %s", name, toString(level));
}

void print(char const* name, Maike::Db::CompilationLog::OutputFormat format)
{
	fprintf(stderr, "%s: %s", name, toString(format));
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
void dumpConfig(Maike::Config const& cfg, Maike::fs::path const&)
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
		Maike::KeyValueStore::init();

		Maike::CommandLine const cmdline{argc - 1, argv + 1};
		if(cmdline.hasOption<Maike::CmdLineOption::Help>())
		{
			printHelp(cmdline);
			return 0;
		}

		if(cmdline.hasOption<Maike::CmdLineOption::Version>())
		{
			printVersion(cmdline.option<Maike::CmdLineOption::Version>());
			return 0;
		}

		if(cmdline.hasOption<Maike::CmdLineOption::About>())
		{
			printAbout(cmdline.option<Maike::CmdLineOption::About>());
			return 0;
		}

		cmdline.forEachOption([](char const* name, auto const& val) {
			print(name, val);
			fputc('\n', stderr);
		});

		auto const build_info = makeBuildInfo(cmdline);

		auto const cfg = Maike::loadConfig(
		   cmdline.hasOption<Maike::CmdLineOption::ConfigFiles>() ?
		      cmdline.option<Maike::CmdLineOption::ConfigFiles>() :
		      std::vector<Maike::fs::path>{build_info.sourceDir() / "maikeconfig.json"});

		if(cmdline.hasOption<Maike::CmdLineOption::ConfigDump>())
		{
			dumpConfig(cfg, cmdline.option<Maike::CmdLineOption::ConfigDump>());
			return 0;
		}

		Logger logger;
		Maike::Sched::ThreadPool workers{cmdline.option<Maike::CmdLineOption::NumWorkers>()};

		Maike::SourceTreeLoader::CommandDictionary commands;
		commands.projectDir(Maike::fs::path{build_info.sourceDir()})
		   .systemDir(Maike::fs::path{Maike::execPrefix() / "lib" / "maike2" / "recipes"});

		auto const cfg_new = resolveRecipes(cfg, commands);
		Maike::SourceTreeLoader::SourceFileLoaderDelegator loader_delegator{commands,
		                                                                    cfg_new.dirCompiler()};
		loader_delegator.loaders(mapSourceFileInfoLoaders(cfg_new));

		printf(
		   "# Build job with %zu workers started\n"
		   "#\n"
		   "# Start time: %s\n"
		   "#         Id: %s\n"
		   "# Source dir: %s\n"
		   "# Target dir: %s\n"
		   "#\n",
		   workers.count(),
		   format(build_info.startTime()).c_str(),
		   toString(build_info.buildId()).c_str(),
		   build_info.sourceDir().c_str(),
		   build_info.targetDir().c_str());
		fflush(stdout);

		auto const src_tree = Maike::timedCall(
		   logger,
		   Maike::SourceTreeLoader::load,
		   workers,
		   build_info.sourceDir(),
		   cfg_new.sourceTreeLoader().inputFilter(),
		   loader_delegator,
		   Maike::SourceTreeLoader::RecursiveScan{cfg_new.sourceTreeLoader().recursive()},
		   build_info.targetDir());

		if(cmdline.hasOption<Maike::CmdLineOption::PrintDepGraph>())
		{
			printDepGraph(src_tree.dependencyGraph(), cmdline.option<Maike::CmdLineOption::PrintDepGraph>());
			return 0;
		}

		Maike::Exec::LocalExecve invoker{cmdline.hasOption<Maike::CmdLineOption::DryRun>()};

		Maike::Db::CompilationLog log{getOutputFormat(cmdline), getLogLevel(cmdline)};

		Maike::timedCall(
		   logger,
		   [](auto&&... args) { return compile(std::forward<decltype(args)>(args)...); },
		   src_tree,
		   build_info,
		   Maike::Db::Invoker{std::ref(invoker)},
		   log,
		   Maike::Db::ForceRecompilation{cmdline.hasOption<Maike::CmdLineOption::ForceRebuild>()},
		   workers);
	}
	catch(std::exception const& err)
	{
		fprintf(stderr, "%s\n", err.what());
		return -1;
	}
}
