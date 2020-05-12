//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "./config.hpp"
#include "./cmd_line_options.hpp"
#include "src/source_tree_loader/directory_scanner.hpp"
#include "src/cxx/source_file_loader.hpp"

#include <chrono>

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
						printf("%s[=`%s`]\n    %s\n\n", item.name(), item.type(), item.summary());
					});
}

int main(int argc, char** argv)
{
	try
	{
		Maike::CommandLine cmdline{argc, argv};
		if(cmdline.hasOption<Maike::CmdLineOption::Help>())
		{
			printHelp(cmdline);
			return 0;
		}

		{
			printf(">>> %s\n", toString(cmdline.option<Maike::CmdLineOption::BuildId>()).c_str());
			return 0;
		}

		Maike::KeyValueStore::init();
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

		Maike::ThreadPool workers;

		std::map<std::string, Maike::SourceTreeLoader::SourceFileLoader> loaders;
		loaders.insert(
		   std::make_pair(std::string{".hpp"},
		                  Maike::SourceTreeLoader::SourceFileLoader{Maike::Cxx::SourceFileLoader{}}));
		loaders.insert(
		   std::make_pair(std::string{".cpp"},
		                  Maike::SourceTreeLoader::SourceFileLoader{Maike::Cxx::SourceFileLoader{}}));

		Maike::SourceTreeLoader::SourceFileLoaderDelegator loader_delegator;
		loader_delegator.loaders(std::move(loaders));

		Maike::SourceTreeLoader::DirectoryScanner scanner{
		   workers, std::cref(cfg.inputFilter()), std::cref(loader_delegator)};
		auto now = std::chrono::steady_clock::now();
		scanner.processPath(Maike::fs::path{"."});
		auto src_files = scanner.takeResult();
		fprintf(stdout,
		        "# Processed %zu files in %.7f seconds\n",
		        src_files.size(),
		        std::chrono::duration<double>(std::chrono::steady_clock::now() - now).count());
		makeSourceFileInfosFromTargets(src_files);
		resolveDependencies(src_files);

#if 0
		std::for_each(std::begin(src_files), std::end(src_files), [](auto const& item) {
			auto const& deps = item.second.usedFiles();
			printf("\"%s\"\n", item.first.c_str());
			std::for_each(std::begin(deps), std::end(deps), [&item](auto const& edge) {
				if(edge.sourceFile() != nullptr)
				{ printf("\"%s\" -> \"%s\"\n", item.first.c_str(), edge.name().c_str()); }
			});
		});
#endif
	}
	catch(std::exception const& err)
	{
		fprintf(stderr, "%s\n", err.what());
		return -1;
	}
}
