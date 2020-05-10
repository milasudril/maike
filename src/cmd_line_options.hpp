//@	{
//@	  "targets":[{"name":"cmd_line_options.hpp","type":"include"}]
//@	}

#ifndef MAIKE_CMDLINEOPTIONS_HPP
#define MAIKE_CMDLINEOPTIONS_HPP

#include "src/cmd_line_parser/command_line.hpp"

namespace Maike
{
	enum class CmdLineOption : int
	{
		Help,
		Version,
		About,
		PrintDepGraph,
		PrintDepGraphInv,
		ListTargets,
		ListLeafTargets,
		ListExternalDeps,
		SelectedTargets,
		ConfigFiles,
		SourceDir,
		TargetDir,
		BuildId,
		StartTime,
		LogFlags,
		LogFile,
		LogFormat,
		Jobs
	};

	static constexpr auto end(Empty<CmdLineOption>)
	{
		return static_cast<int>(CmdLineOption::PrintDepGraph) + 1;
	}

	template<CmdLineOption opt>
	struct CmdLineOptionTraits;

	template<>
	struct CmdLineOptionTraits<CmdLineOption::Help>
	{
		using type = Maike::fs::path;

		static constexpr char const* category()
		{
			return "Program information";
		}

		static constexpr char const* summary()
		{
			return "Prints this message to stdout and exits, or `filename`, and exits";
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
		using type = Maike::fs::path;

		static constexpr char const* category()
		{
			return "Program information";
		}

		static constexpr char const* summary()
		{
			return "Prints version information about the program to stdout or `filename`, and exists";
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
		using type = std::false_type;

		static constexpr char const* category()
		{
			return "Program information";
		}

		static constexpr bool exclusive()
		{
			return true;
		}

		static constexpr char const* summary()
		{
			return "Prints detailed information about the program to stdout or `filename`, and exits";
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
		using type = std::false_type;

		static constexpr char const* category()
		{
			return "Database queries";
		}

		static constexpr bool exclusive()
		{
			return true;
		}

		static constexpr char const* summary()
		{
			return "Prints the dependency graph as a dot(1) file to stdout or `filename` and exits";
		}

		static constexpr char const* description()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--print-dep-graph";
		}
	};

	using CommandLine = CmdLineParser::BasicCommandLine<CmdLineOption, CmdLineOptionTraits>;
}

#endif