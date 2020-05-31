//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "./cmd_line_options.hpp"
#include "./build_info.hpp"

#include "src/db/target.hpp"
#include "src/io/input_file.hpp"
#include "src/config/main.hpp"
#include "src/source_tree_loader/directory_scanner.hpp"
#include "src/source_file_info_loaders/cxx/source_file_loader.hpp"

void makeSourceFileInfosFromTargets(Maike::Db::SourceFileIndex& source_files,
                                    Maike::fs::path const& target_dir)
{
	source_files.visitByPath([&source_files, target_dir](auto const& item) {
		auto const& src_file_info = item.sourceFileInfo();
		auto const& targets = src_file_info.targets();
		std::for_each(std::begin(targets),
		              std::end(targets),
		              [&source_files, &name = item.path(), &target_dir](auto const& target) {
			              if(name != target) // For backwards compatiblity with old maike
			              {
				              auto i = source_files.get(target);
				              if(i.valid()) { throw std::runtime_error{"Target has already been defined"}; }

				              Maike::Db::SourceFileInfo src_file;
				              source_files.insert(target_dir / target, std::move(src_file));
			              }
		              });
	});
}

std::map<Maike::fs::path, Maike::Db::Target>
collectTargets(Maike::Db::SourceFileIndex const& source_files, Maike::fs::path const& target_dir)
{
	std::map<Maike::fs::path, Maike::Db::Target> ret;

	source_files.visitByPath([&ret, target_dir](auto const& item) {
		auto const& src_file_info = item.sourceFileInfo();
		auto const& targets = src_file_info.targets();
		std::for_each(
		   std::begin(targets), std::end(targets), [&ret, &item, &target_dir](auto const& target) {
			   if(item.path() != target) // For backwards compatiblity with old maike
			   {
				   auto i = ret.find(target);
				   if(i != std::end(ret)) { throw std::runtime_error{"Target has already been defined"}; }

				   ret.insert(i,
				              std::make_pair(target_dir / target,
				                             Maike::Db::Target{item.path(), item.sourceFileInfo()}));
			   }
		   });
	});
	return ret;
}

#if 0
void resolveDependencies(std::map<Maike::fs::path, Maike::SourceFileInfo>& source_files)
{
	std::for_each(std::begin(source_files), std::end(source_files), [&source_files](auto& item) {
		auto const& deps = item.second.useDeps();
		std::vector<Maike::Dependency> deps_resolved;
		deps_resolved.reserve(deps.size());
		std::transform(std::begin(deps),
		               std::end(deps),
		               std::back_inserter(deps_resolved),
		               [&source_files](auto const& edge) {
			               auto tmp = edge;
			               return tmp.resolve(source_files);
		               });
		item.second.useDeps(std::move(deps_resolved));
	});
}
#endif

void printDepGraph(std::map<Maike::fs::path, Maike::Db::Target> const& targets,
                   Maike::Db::SourceFileIndex const& source_files,
                   Maike::fs::path const&)
{
	puts("digraph \"G\" {\nrankdir=LR\n");
	std::for_each(std::begin(targets), std::end(targets), [](auto const& item) {
		printf("\"%s\" -> \"%s\"\n", item.first.c_str(), item.second.sourceFilename().c_str());
	});


	source_files.visitByPath([](auto const& item) {
		auto const& deps = item.sourceFileInfo().useDeps();
		printf("\"%s\"\n", item.path().c_str());
		std::for_each(std::begin(deps), std::end(deps), [&item](auto const& edge) {
			printf("\"%s\" -> \"%s\"\n", item.path().c_str(), edge.name().c_str());
		});
	});
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


		Maike::Sched::ThreadPool workers{cmdline.option<Maike::CmdLineOption::NumWorkers>()};

		Maike::SourceTreeLoader::SourceFileLoaderDelegator loader_delegator;
		loader_delegator.loaders(mapSourceFileInfoLoaders(cfg));

		Maike::SourceTreeLoader::DirectoryScanner scanner{
		   workers, std::cref(cfg.sourceTreeLoader().inputFilter()), std::cref(loader_delegator)};
		auto now = std::chrono::steady_clock::now();
		scanner.processPath(Maike::fs::path{"."});
		auto src_files = scanner.takeResult();
		fprintf(stderr,
		        "# Processed %zu files in %.7f seconds\n",
		        src_files.size(),
		        std::chrono::duration<double>(std::chrono::steady_clock::now() - now).count());
		auto const target_dir = cmdline.hasOption<Maike::CmdLineOption::TargetDir>() ?
		                           cmdline.option<Maike::CmdLineOption::TargetDir>() :
		                           Maike::fs::path{"__targets"};
		makeSourceFileInfosFromTargets(src_files, target_dir);

		auto targets = collectTargets(src_files, target_dir);
		if(cmdline.hasOption<Maike::CmdLineOption::PrintDepGraph>())
		{
			printDepGraph(targets, src_files, cmdline.option<Maike::CmdLineOption::PrintDepGraph>());
			return 0;
		}
#if 0
		resolveDependencies(src_files);
#endif
	}
	catch(std::exception const& err)
	{
		fprintf(stderr, "%s\n", err.what());
		return -1;
	}
}
