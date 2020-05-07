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
		PrintHelp,
		PrintVersion,
		PrintAbout,
		PrintDepGraph,
		PrintDepGraphInv,
		ListTargets,
		ListLeafTargets,
		ListExternalDeps,
		SelectedTargets,
		ConfigFiles,
		SourceDir,
		TargetDir,
		LogFlags,
		LogFile,
		BuildId,
		StartTime
	};

	static constexpr auto end(Empty<CmdLineOption>)
	{
		return static_cast<int>(CmdLineOption::PrintAbout) + 1;
	}

	template<CmdLineOption opt>
	struct CmdLineOptionTraits;

	template<>
	struct CmdLineOptionTraits<CmdLineOption::PrintHelp>
	{
		using type = std::false_type;

		static constexpr bool exclusive()
		{
			return true;
		}

		static constexpr char const* summary()
		{
			return "Prints this message to stdout and exits";
		}

		static constexpr char const* detailedDescription()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--help";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::PrintVersion>
	{
		using type = std::false_type;

		static constexpr bool exclusive()
		{
			return true;
		}

		static constexpr char const* summary()
		{
			return "Prints version information about the program";
		}

		static constexpr char const* detailedDescription()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--version";
		}
	};

	template<>
	struct CmdLineOptionTraits<CmdLineOption::PrintAbout>
	{
		using type = std::false_type;

		static constexpr bool exclusive()
		{
			return true;
		}

		static constexpr char const* summary()
		{
			return "Prints detailed information about the program";
		}

		static constexpr char const* detailedDescription()
		{
			return nullptr;
		}

		static constexpr char const* name()
		{
			return "--about";
		}
	};

	using CommandLine = CmdLineParser::BasicCommandLine<CmdLineOption, CmdLineOptionTraits>;
}

#endif