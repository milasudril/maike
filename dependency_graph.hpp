//@	{
//@	  "targets":[{"name":"dependency_graph.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"dependency_graph.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DEPENDENCYGRAPH_HPP
#define MAIKE_DEPENDENCYGRAPH_HPP

#include "./source_file_info.hpp"

#include <map>
#include <algorithm>

namespace Maike
{
	class DependencyGraph
	{
	public:
		SourceFile<NonConstTag> insert(fs::path&& src_file_name, SourceFileInfo&& src_file_info);

		Maike::SourceFile<ConstTag> find(fs::path const& src_file) const;

		Maike::SourceFile<NonConstTag> find(fs::path const& src_file);

		template<class Visitor>
		void visitItems(Visitor&& v) const
		{
			std::for_each(std::begin(m_sources), std::end(m_sources), [&v](auto const& item) {
				v(SourceFile<ConstTag>{item.first, item.second});
			});
		}

		template<class Visitor>
		void visitItems(Visitor&& v)
		{
			std::for_each(std::begin(m_sources), std::end(m_sources), [&v](auto& item) {
				v(SourceFile<NonConstTag>{item.first, item.second});
			});
		}

	private:
		std::map<fs::path, Maike::SourceFileInfo> m_sources;
	};
}

#endif