//@	{
//@	  "targets":[{"name":"dependency_graph.o","type":"object"}]
//@	 }

#include "./dependency_graph.hpp"

#include "core/utils/graphutils.hpp"

#include <algorithm>

namespace
{
	struct CompareSourceFileRecord
	{
		bool operator()(Maike::fs::path const& a, Maike::Db::SourceFileRecordConst const& b)
		{
			return a < b.path();
		}

		bool operator()(Maike::Db::SourceFileRecordConst const& a, Maike::fs::path const& b)
		{
			return a.path() < b;
		}
	};

	template<class ForwardIt, class T, class Compare>
	ForwardIt find(ForwardIt first, ForwardIt last, const T& value, Compare cmp)
	{
		ForwardIt i = std::lower_bound(first, last, value, cmp);
		if(i != last && !cmp(value, *i))
			return i;
		else
			return last;
	}

	void resolve(Maike::Db::Dependency& dep,
	             std::vector<Maike::Db::SourceFileRecordConst> const& src_files,
	             Maike::Db::DependencyGraph::IgnoreResolveErrors ignore_errors,
	             Maike::fs::path const& required_by)
	{
		auto i = find(std::begin(src_files), std::end(src_files), dep.name(), CompareSourceFileRecord{});
		if(i == std::end(src_files))
		{
			if(!ignore_errors)
			{
				std::string msg{required_by};
				msg += ": error: ";
				msg += "Failed to resolve ";
				msg += dep.name().string();
				throw std::runtime_error{msg};
			}
			return;
		}
		dep.reference(i->id());
	}
}

Maike::Db::DependencyGraph::DependencyGraph(std::map<fs::path, SourceFileInfo>&& src_files,
                                            IgnoreResolveErrors ignore_res_errors):
   m_src_files{std::move(src_files)}
{
	m_nodes.reserve(m_src_files.size());
	std::transform(
	   std::begin(m_src_files),
	   std::end(m_src_files),
	   std::back_inserter(m_nodes),
	   [k = static_cast<size_t>(-1)](auto const& item) mutable {
		   ++k;
		   return SourceFileRecordConst{SourceFileId{k}, std::cref(item.first), std::cref(item.second)};
	   });

	std::for_each(std::begin(m_src_files),
	              std::end(m_src_files),
	              [&nodes = m_nodes, ignore_res_errors](auto& item) {
		              {
			              auto use_deps = item.second.useDeps();
			              auto build_deps = item.second.buildDeps();
			              std::for_each(std::begin(use_deps),
			                            std::end(use_deps),
			                            [&nodes, ignore_res_errors, &required_by = item.first](auto& item) {
				                            resolve(item, nodes, ignore_res_errors, required_by);
			                            });
			              std::for_each(std::begin(build_deps),
			                            std::end(build_deps),
			                            [&nodes, ignore_res_errors, &required_by = item.first](auto& item) {
				                            resolve(item, nodes, ignore_res_errors, required_by);
			                            });
			              item.second.useDeps(std::move(use_deps)).buildDeps(std::move(build_deps));
		              }
	              });
}


Maike::Db::SourceFileRecordConst Maike::Db::getNode(DependencyGraph const& g,
                                                    Maike::fs::path const& name)
{
	auto& nodes = g.nodes();
	auto i = find(std::begin(nodes), std::end(nodes), name, CompareSourceFileRecord{});
	if(i == std::end(nodes)) { return SourceFileRecordConst{}; }
	return *i;
}

std::vector<Maike::Db::Dependency> Maike::Db::getUseDepsRecursive(DependencyGraph const& g,
                                                                  SourceFileRecordConst const& rec)
{
	std::vector<Db::Dependency> ret;
	Maike::processGraphNodeRecursive(
	   [&ret](auto const&, Db::Dependency const* dep) {
		   if(dep != nullptr && dep->name() != "") { ret.push_back(*dep); }
	   },
	   g,
	   rec);
	return ret;
}

void Maike::Db::compile(DependencyGraph const& g,
                        SourceFileRecordConst const& node,
                        Build::Info const& build_info,
                        Invoker invoker,
                        CompilationLog& log,
                        ForceRecompilation force_recompilation,
                        Sched::Batch const& ctxt)
{
	// No targets. Nothing to do.
	if(std::size(node.sourceFileInfo().targets()) == 0) { return; }

	CompilationLog::EntryContext log_entry{log};
	log_entry.sourceFileId(node.id()).sourcePath(fs::path{node.path()});

	// Generate the command while we're waiting for dependencies to complete
	auto use_deps = getUseDepsRecursive(g, node);
	auto cmd = makeBuildCommand(node, build_info, use_deps, log.outputFormat());

	// Wait until build deps has been processed
	auto const& build_deps = node.sourceFileInfo().buildDeps();
	if(std::any_of(std::begin(build_deps), std::end(build_deps), [&ctxt](auto const& item) {
		   return ctxt.taskFailed(item.reference().value());
	   }))
	{
		std::string msg{node.path()};
		msg += ": At least one dependency was not compiled";
		throw std::runtime_error{std::move(msg)};
	}

	// Wait until use deps for all build deps have been processed
	if(std::any_of(std::begin(use_deps), std::end(use_deps), [&ctxt](auto const& item) {
		   return ctxt.taskFailed(item.reference().value());
	   }))
	{
		std::string msg{node.path()};
		msg += ": At least one dependency was not compiled";
		throw std::runtime_error{std::move(msg)};
	}

	if(force_recompilation || node.sourceFileInfo().rebuildPolicy() == RebuildPolicy::Always
	   || !isUpToDate(node, use_deps))
	{
		auto result = invoker.execve(cmd);
		log_entry.command(std::move(cmd));
		auto const build_failed = failed(result);
		log_entry.result(std::move(result));

		if(build_failed)
		{
			std::string msg{node.path()};
			msg += ":1:1: Compilation failed";
			throw std::runtime_error{std::move(msg)};
		}
	}
}