//@	{
//@	 "dependencies_extra":[{"ref":"./compilation_log.o", "rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_COMPILATIONLOG_HPP
#define MAIKE_DB_COMPILATIONLOG_HPP

#include "./task.hpp"

#include "core/exec/result.hpp"
#include "core/exec/command.hpp"
#include "core/key_value_store/json_handle.hpp"
#include "core/utils/empty.hpp"

#include <chrono>
#include <mutex>

namespace Maike::Db
{
	class CompilationLog
	{
	public:
		using OutputFormat = LogFormat;

		enum class LogLevel : int
		{
			Errors,
			SourceFileInfo,
			CompilationCommand
		};

		explicit CompilationLog(LogFormat format, LogLevel log_level):
		   m_format{format}, m_log_level{log_level}, m_start_time{std::chrono::steady_clock::now()}
		{
		}

		LogFormat outputFormat() const
		{
			return m_format;
		}

		CompilationLog& write(Task const& task, Exec::Result const& result);

		CompilationLog& write(Task const& task);

	private:
		LogFormat m_format;
		LogLevel m_log_level;
		std::chrono::steady_clock::time_point m_start_time;
		std::mutex m_output_mutex;
	};

	char const* toString(CompilationLog::LogLevel);

	CompilationLog::LogLevel fromString(Empty<CompilationLog::LogLevel>, char const*);


	constexpr char const* typeToString(Empty<CompilationLog::LogLevel>)
	{
		return "Log level";
	}

	constexpr char const* typeDescription(Empty<CompilationLog::LogLevel>)
	{
		return "A *Log level* takes one of the values *errors*, *source_file_info*, and "
		       "*compilation_command*. "
		       "\n\n"
		       "* *errors* will only print error messages\n\n"
		       "* *source_file_info* will print information about the source file being compiled\n\n"
		       "* *compilation_command* will show the compilation command used for each file being "
		       "compiled\n\n"
		       "Default value is *source_file_info*. Notice that *errors* are written to *stderr*, "
		       "while other messages are written to *stdout*.";
	}
}

#endif
