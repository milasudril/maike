//@	{
//@	  "targets":[{"name":"compilation_log.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"compilation_log.o", "rel":"implementation"}]
//@	 }

#ifndef MAIKE_DB_COMPILATIONLOG_HPP
#define MAIKE_DB_COMPILATIONLOG_HPP

#include "./source_file_id.hpp"

#include "src/exec/result.hpp"
#include "src/exec/command.hpp"
#include "src/key_value_store/json_handle.hpp"
#include "src/utils/empty.hpp"

#include <chrono>
#include <forward_list>
#include <mutex>

namespace Maike::Db
{
	class CompilationLog
	{
	public:
		enum class OutputFormat : int
		{
			PlainText,
			AnsiTerm
		};

		enum class LogLevel : int
		{
			Errors,
			SourceFileInfo,
			CompilationCommand
		};

		explicit CompilationLog(OutputFormat format, LogLevel log_level):
		   m_format{format}, m_log_level{log_level}, m_start_time{std::chrono::steady_clock::now()}
		{
		}

		OutputFormat outputFormat() const
		{
			return m_format;
		}

		struct Entry
		{
			Entry(): result{Exec::ExitStatus::success()}
			{
			}

			std::chrono::steady_clock::time_point start_time;
			std::chrono::steady_clock::time_point completion_time;
			SourceFileId src_id;
			fs::path src_path;
			Exec::Command command;
			Exec::Result result;
		};

		CompilationLog& write(Entry&& e);

		class EntryContext
		{
		public:
			EntryContext(EntryContext&&) = delete;
			EntryContext& operator=(EntryContext&&) = delete;

			explicit EntryContext(CompilationLog& log): m_log{log}
			{
				m_entry.start_time = std::chrono::steady_clock::now();
			}

			~EntryContext()
			{
				m_entry.completion_time = std::chrono::steady_clock::now();
				m_log.write(std::move(m_entry));
			}

			EntryContext& sourceFileId(SourceFileId id)
			{
				m_entry.src_id = id;
				return *this;
			}

			EntryContext& sourcePath(fs::path&& path)
			{
				m_entry.src_path = std::move(path);
				return *this;
			}

			EntryContext& command(Exec::Command&& cmd)
			{
				m_entry.command = std::move(cmd);
				return *this;
			}

			EntryContext& result(Exec::Result&& res)
			{
				m_entry.result = std::move(res);
				return *this;
			}

		private:
			Entry m_entry;
			CompilationLog& m_log;
		};

	private:
		OutputFormat m_format;
		LogLevel m_log_level;
		std::chrono::steady_clock::time_point m_start_time;
		std::mutex m_output_mutex;
	};

	char const* toString(CompilationLog::OutputFormat format);

	char const* toString(CompilationLog::LogLevel format);

	inline KeyValueStore::JsonHandle toJson(CompilationLog::OutputFormat format)
	{
		return KeyValueStore::toJson(toString(format));
	}

	CompilationLog::OutputFormat fromString(Empty<CompilationLog::OutputFormat>, char const*);

	CompilationLog::LogLevel fromString(Empty<CompilationLog::LogLevel>, char const*);


	constexpr char const* typeToString(Empty<CompilationLog::LogLevel>)
	{
		return "LogLevel";
	}

	constexpr char const* typeToString(Empty<CompilationLog::OutputFormat>)
	{
		return "OutputFormat";
	}
}

#endif