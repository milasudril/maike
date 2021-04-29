//@	{
//@	  "targets":[{"name":"compilation_log.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_COMPILATIONLOG_HPP
#define MAIKE_DB_COMPILATIONLOG_HPP

#include "src/exec/command.hpp"

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

		explicit CompilationLog(OutputFormat format): m_format{format}
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

		CompilationLog& write(Entry&& e)
		{
			std::lock_guard lock{m_list_mutex};
			m_entries.push_front(std::move(e));
			return *this;
		}

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
		std::mutex m_list_mutex;
		std::forward_list<Entry> m_entries;
	};
}

#endif