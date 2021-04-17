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

void Maike::Db::compile(SourceFileRecordConst const& rec,
                        Build::Info const& info,
                        std::vector<Dependency> const&)
{
	auto& targets = rec.sourceFileInfo().targets();
	if(std::size(targets) == 0) { return; }

	KeyValueStore::Compound cmd_opts;
	cmd_opts.set("compiler_config", rec.sourceFileInfo().compiler().config())
	   .set("source_file", rec.path().c_str())
	   .set("task_id", rec.id().value())
	   .set("build_info", info);

	KeyValueStore::Array target_names;
	std::for_each(std::begin(targets), std::end(targets), [&target_names](auto const& item) {
		target_names.append(item.name().c_str());
	});
	cmd_opts.set("targets", std::move(target_names));

	CmdOptsString str;
	store(cmd_opts, str);


	//	.set("targets", targets[0]);
	//	.set("");


	std::lock_guard lock{m};
	printf("%s\n", str.buffer.c_str());
}