//@	{
//@	 "targets":[{"name":"spiderdefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"spiderdefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SPIDERDEFAULT_HPP
#define MAIKE_SPIDERDEFAULT_HPP

#include "spider.hpp"
#include "twins.hpp"
#include <stack>
#include <set>
#include <string>

namespace Maike
	{
	class DependencyGraph;
	class Target_FactoryDelegator;

	class PRIVATE SpiderDefault:public Spider
		{
		public:
			explicit
			SpiderDefault(Target_FactoryDelegator& deleg
				,DependencyGraph& targets);

			SpiderDefault& scanFile(const char* filename,const char* in_dir);
			SpiderDefault& run();

		private:
			Target_FactoryDelegator& r_target_creator;
			DependencyGraph& r_targets;

			typedef Stringkey VisitedKey;
			std::stack< Twins<std::string> > m_files_to_scan;
			std::set<VisitedKey> m_files_visited;
		};
	}

#endif
