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
		LogFlags,
		LogFile,
		BuildId,
		StartTime
	};

	static constexpr auto end(Empty<CmdLineOption>)
	{
		return static_cast<int>(CmdLineOption::About) + 1;
	}

	template<CmdLineOption opt>
	struct CmdLineOptionTraits;

	template<>
	struct CmdLineOptionTraits<CmdLineOption::Help>
	{
		using type = Maike::fs::path;

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
	struct CmdLineOptionTraits<CmdLineOption::Version>
	{
		using type = Maike::fs::path;

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
	struct CmdLineOptionTraits<CmdLineOption::About>
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