//@	{
//@	  "targets":[{"name":"dependency_graph.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"dependency_graph.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DEPENDENCYGRAPH_HPP
#define MAIKE_DEPENDENCYGRAPH_HPP

#include "./source_file.hpp"

#include <set>
#include <algorithm>

namespace Maike
{
	class DependencyGraph
	{
		public:
			Maike::SourceFile const& insert(SourceFile&& src_file);

			Maike::SourceFile const* find(SourceFile const& src_file) const;

			Maike::SourceFile const* find(fs::path const& path) const;

			template<class Visitor>
			void visitItems(Visitor&& v) const
			{
				std::for_each(std::begin(m_sources), std::end(m_sources), std::forward<Visitor>(v));
			}

		private:
			struct SourceFileByName
			{
				using is_transparent = void;

				bool operator()(SourceFile const& a, SourceFile const& b) const
				{
					return a.name() < b.name();
				}

				bool operator()(SourceFile const& a, fs::path const& b) const
				{
					return a.name() < b;
				}

				bool operator()(fs::path const& a, SourceFile const& b) const
				{
					return a < b.name();
				}
			};

			std::set<Maike::SourceFile, SourceFileByName> m_sources;
	};
}

#endif