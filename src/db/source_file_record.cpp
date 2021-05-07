//@	{
//@	  "targets":[{"name":"source_file_record.o","type":"object"}]
//@	}

#include "./source_file_record.hpp"

#include "src/key_value_store/array.hpp"

#include <algorithm>
#include <mutex>

bool Maike::Db::isUpToDate(SourceFileRecordConst const& rec, std::vector<Dependency> const& deps)
{
	auto const& targets = rec.sourceFileInfo().targets();
	return std::all_of(
	   std::begin(targets), std::end(targets), [&src_file = rec.path(), &deps](auto const& item) {
		   return isUpToDate(item, src_file, deps);
	   });
}

static std::mutex m;

namespace
{
	struct CmdOptsString
	{
		std::string buffer;
	};

	void write(CmdOptsString& str, char const* vals, size_t n)
	{
		std::copy_n(vals, n, std::back_inserter(str.buffer));
	}
}

Maike::Exec::Command Maike::Db::makeBuildCommand(SourceFileRecordConst const& rec,
                                                 Build::Info const& info,
                                                 std::vector<Dependency> const& deps,
                                                 CompilationLog::OutputFormat format)
{
	auto& targets = rec.sourceFileInfo().targets();

	auto compiler = rec.sourceFileInfo().compilerDefault() | rec.sourceFileInfo().compiler();

	KeyValueStore::Compound cmd_opts;
	cmd_opts.set("compiler_cfg", compiler.config())
	   .set("source_file", rec.path().c_str())
	   .set("task_id", rec.id().value())
	   .set("build_info", info)
	   .set("log_format", format);

	{
		KeyValueStore::Array target_names;
		std::for_each(std::begin(targets), std::end(targets), [&target_names](auto const& item) {
			target_names.append(item.name().c_str());
		});
		cmd_opts.set("targets", std::move(target_names));
	}

	{
		KeyValueStore::Array deps_out;
		std::for_each(
		   std::begin(deps), std::end(deps), [&deps_out](auto const& item) { deps_out.append(item); });
		cmd_opts.set("dependencies", std::move(deps_out));
	}

	CmdOptsString str;
	store(cmd_opts, str, KeyValueStore::PrettyPrint{false});

	auto recipe = compiler.recipe() == "" ? rec.path() : std::move(compiler.recipe());

	return Exec::Command{std::move(recipe),
	                     std::vector<std::string>{"compile", std::move(str.buffer)}};
}
