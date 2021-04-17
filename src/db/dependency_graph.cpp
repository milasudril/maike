//@	{
//@	  "targets":[{"name":"dependency_graph.o","type":"object"}]
//@	 }

#include "./dependency_graph.hpp"

#include "src/utils/graphutils.hpp"

#include <algorithm>
#include <set>

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
	             Maike::Db::DependencyGraph::IgnoreResolveErrors ignore_errors)
	{
		auto i = find(std::begin(src_files), std::end(src_files), dep.name(), CompareSourceFileRecord{});
		if(i == std::end(src_files))
		{
			if(!ignore_errors)
			{ throw std::runtime_error{std::string{"Failed to resolve "} + dep.name().string()}; }
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
			                            [&nodes, ignore_res_errors](auto& item) {
				                            resolve(item, nodes, ignore_res_errors);
			                            });
			              std::for_each(std::begin(build_deps),
			                            std::end(build_deps),
			                            [&nodes, ignore_res_errors](auto& item) {
				                            resolve(item, nodes, ignore_res_errors);
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
	std::set<fs::path> bookkeeping;
	Maike::processGraphNodeRecursive(
	   [&ret, &bookkeeping](auto const& node) {
		   auto i = bookkeeping.find(node.path());
		   if(i != std::end(bookkeeping)) { return; }
		   bookkeeping.insert(i, node.path());
		   ret.push_back(
		      Db::Dependency{node.path(), node.sourceFileInfo().origin()}.reference(node.id()));
	   },
	   g,
	   rec,
	   UseDepsOnly{});
	ret.pop_back();

	return ret;
}

void Maike::Db::compile(DependencyGraph const& g,
                        SourceFileRecordConst const& node,
                        Build::Info const& build_info,
                        ForceRecompilation force_recompilation,
                        Sched::Batch const& ctxt)
{
	auto use_deps = getUseDepsRecursive(g, node);
	if(std::any_of(std::begin(use_deps), std::end(use_deps), [&ctxt](auto const& item) {
		   return ctxt.taskFailed(item.reference().value());
	   }))
	{
		std::string msg{node.path()};
		msg += ": At least one dependency was not compiled";
		throw std::runtime_error{std::move(msg)};
	}
	if(force_recompilation || !isUpToDate(node, use_deps)) { compile(node, build_info, use_deps); }
}