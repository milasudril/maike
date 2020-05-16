//@	{
//@	  "targets":[{"name":"cmd_line_options.hpp","type":"include"}]
//@	}

#ifndef MAIKE_CMDLINEOPTIONS_HPP
#define MAIKE_CMDLINEOPTIONS_HPP

#include "./fs.hpp"
#include "./build_id.hpp"
#include "./system_time_stamp.hpp"

#include "src/sched/thread_count.hpp"
#include "src/cmd_line_parser/command_line.hpp"

#include <vector>

namespace Maike
{
	enum class CmdLineOption : int
	{
		Help,
		Version,
		About,
		PrintDepGraph,
		PrintDepGraphInv,
		DumpDatabase,
		ListLeafTargets,
		ListExternalDeps,
		Targets,
		ConfigFiles,
		ConfigDump,
		SourceDir,
		TargetDir,
		BuildId,
		StartTime,
		NumWorkers,
		DryRun
		//	LogFlags,
		//	LogFile,
		//	LogFormat,
	};

	static constexpr auto end(Empty<CmdLineOption>)
	{
		return static_cast<int>(CmdLineOption::DryRun) + 1;
	}

	template<CmdLineOption opt>
	struct CmdLineOptionTraits;

	template<>
	struct CmdLineOptionTraits<CmdLineOption::Help>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Program information";
		}

		static constexpr char const* summary()
		{
			return "Prints this message to stdout and exits, or `path`, and exits";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--help";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::Version>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Program information";
		}

		static constexpr char const* summary()
		{
			return "Prints version information about the program to stdout or `path`, and exists";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--version";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::About>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Program information";
		}

		static constexpr char const* summary()
		{
			return "Prints detailed information about the program to stdout or `path`, and exits";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--about";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::PrintDepGraph>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Database queries";
		}

		static constexpr char const* summary()
		{
			return "Prints the forward dependency graph as a dot(1) file to stdout or `path`, and "
			       "exits";
		}

		static constexpr char const* description()
		{
			return "The forward dependency graph starts at a node, and shows all its dependencies "
			       "recursively.";
		}

		static constexpr char const* name()
		{
			return "--print-dep-graph";
		}
	};


	template<>
	struct CmdLineOptionTraits<CmdLineOption::PrintDepGraphInv>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Database queries";
		}

		static constexpr char const* summary()
		{
			return "Prints the backward dependency graph as a dot(1) file to stdout or `path`, and "
			       "exits";
		}

		static constexpr char const* description()
		{
			return "The backward dependency graph starts at a node, and shows all nodes that depends on it.";
		}

		static constexpr char const* name()
		{
			return "--print-dep-graph-inv";
		}
	};


	template<>
	struct CmdLineOptionTraits<CmdLineOption::DumpDatabase>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Database queries";
		}

		static constexpr char const* summary()
		{
			return "Dumps the loaded database as JSON to stdout or `path`, and exits";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--dump-database";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::ListLeafTargets>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Database queries";
		}

		static constexpr char const* summary()
		{
			return "Prints a list of all leaf targets to stdout or `path`, and exits";
		}

		static constexpr char const* description()
		{
			return "A leaf target is a target that no other target is dependent on";
		}

		static constexpr char const* name()
		{
			return "--list-leaf-targets";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::ListExternalDeps>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Database queries";
		}
		static constexpr char const* summary()
		{
			return "Prints a list of all external dependencies to stdout or `path`, and exits";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--list-external-deps";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::BuildId>
	{
		using type = BuildId;

		static constexpr bool valueRequired()
		{
			return true;
		}

		static constexpr char const* category()
		{
			return "Job parameters";
		}

		static constexpr char const* summary()
		{
			return "Specifies a fixed build id";
		}

		static constexpr char const* description()
		{
			return "A build id is intendend to uniqely identify a build job. Specifying a build id is "
			       "useful when the job is part of a larger build process, and the build id has been "
			       "generated in an earlier stage. A hardcoded build id is also useful when debugging code "
			       "that depends on the current build id.";
		}

		static constexpr char const* name()
		{
			return "--build-id";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::Targets>
	{
		using type = std::vector<fs::path>;

		static constexpr bool valueRequired()
		{
			return true;
		}

		static constexpr char const* category()
		{
			return "Job parameters";
		}

		static constexpr char const* summary()
		{
			return "Only process the listed targets";
		}

		static constexpr char const* description()
		{
			return "By default, maike will process all targets.";
		}

		static constexpr char const* name()
		{
			return "--targets";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::ConfigFiles>
	{
		using type = std::vector<fs::path>;

		static constexpr bool valueRequired()
		{
			return true;
		}

		static constexpr char const* category()
		{
			return "Configuration";
		}

		static constexpr char const* summary()
		{
			return "Use project specific settings stored in `paths`";
		}

		static constexpr char const* description()
		{
			return "Settings in a later is merged to any previously loaded configuration. By default, maike "
			       "will load settings from `maikeconfig.json` in found in `source-dir`. If that file "
			       "is not found, a builtin default configuration will be used.";
		}

		static constexpr char const* name()
		{
			return "--configfiles";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::ConfigDump>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Configuration";
		}

		static constexpr char const* summary()
		{
			return "Dump current configuration in JSON format to stdout, or `path`";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--configdump";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::SourceDir>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return true;
		}

		static constexpr char const* category()
		{
			return "Job parameters";
		}

		static constexpr char const* summary()
		{
			return "Process the source tree given by `path`";
		}

		static constexpr char const* description()
		{
			return "By default, current working directory is used";
		}

		static constexpr char const* name()
		{
			return "--source-dir";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::TargetDir>
	{
		using type = fs::path;

		static constexpr bool valueRequired()
		{
			return true;
		}

		static constexpr char const* category()
		{
			return "Job parameters";
		}

		static constexpr char const* summary()
		{
			return "Store output files in `path`";
		}

		static constexpr char const* description()
		{
			return "By default, `__targets` is used.";
		}

		static constexpr char const* name()
		{
			return "--target-dir";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::NumWorkers>
	{
		using type = ThreadCount;

		static constexpr bool valueRequired()
		{
			return true;
		}

		static constexpr char const* category()
		{
			return "Processing options";
		}

		static constexpr char const* summary()
		{
			return "Specifies the number of worker threads to start";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--num-workers";
		}
	};


	template<>
	struct CmdLineOptionTraits<CmdLineOption::DryRun>
	{
		using type = std::false_type;

		static constexpr bool valueRequired()
		{
			return false;
		}

		static constexpr char const* category()
		{
			return "Processing options";
		}

		static constexpr char const* summary()
		{
			return "Only print commands that would be executed, but do not execute them";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--dry-run";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::StartTime>
	{
		using type = SystemTimeStamp;

		static constexpr bool valueRequired()
		{
			return true;
		}

		static constexpr char const* category()
		{
			return "Job parameters";
		}

		static constexpr char const* summary()
		{
			return "Specifies a fixed start time";
		}

		static constexpr char const* description()
		{
			return "The start time indicates when the build job was started. Specifying a start time is "
			       "useful when the job is part of a larger build process, and the main job has been "
			       "started earlier.";
		}

		static constexpr char const* name()
		{
			return "--start-time";
		}
	};

	using CommandLine = CmdLineParser::BasicCommandLine<CmdLineOption, CmdLineOptionTraits>;
}

#endif