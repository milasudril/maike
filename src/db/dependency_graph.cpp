//@	{
//@	  "targets":[{"name":"dependency_graph.o","type":"object"}]
//@	 }

#include "./dependency_graph.hpp"

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
	             std::vector<Maike::Db::SourceFileRecordConst> const& src_files)
	{
		switch(dep.resolver())
		{
			case Maike::Db::Dependency::Resolver::InternalLookup:
			{
				auto i =
				   find(std::begin(src_files), std::end(src_files), dep.name(), CompareSourceFileRecord{});
				if(i == std::end(src_files))
				{ throw std::runtime_error{std::string{"Failed to resolve "} + dep.name().string()}; }

				dep.reference(i->id());
			}
			break;
			case Maike::Db::Dependency::Resolver::None: break;
			case Maike::Db::Dependency::Resolver::PkgConfig: break;
		}
	}
}

Maike::Db::DependencyGraph::DependencyGraph(std::map<fs::path, SourceFileInfo>&& src_files):
   m_src_files{std::move(src_files)}
{
	// TODO: Create placeholder "SourceFileInfo" so to represent external deps

	m_nodes.reserve(m_src_files.size());
	std::transform(
	   std::begin(m_src_files),
	   std::end(m_src_files),
	   std::back_inserter(m_nodes),
	   [k = static_cast<size_t>(-1)](auto const& item) mutable {
		   ++k;
		   return SourceFileRecordConst{SourceFileId{k}, std::cref(item.first), std::cref(item.second)};
	   });

	std::for_each(std::begin(m_src_files), std::end(m_src_files), [&nodes = m_nodes](auto& item) {
		auto deps = item.second.useDepsCopy();
		std::for_each(std::begin(deps), std::end(deps), [&nodes](auto& item) { resolve(item, nodes); });
		item.second.useDeps(std::move(deps));
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