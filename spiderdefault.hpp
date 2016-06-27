//@	{
//@	 "targets":[{"name":"spiderdefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"spiderdefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SPIDERDEFAULT_HPP
#define MAIKE_SPIDERDEFAULT_HPP

#include "spider.hpp"
#include "stringkey.hpp"
#include "twins.hpp"
#include <stack>
#include <set>
#include <map>
#include <string>

namespace Maike
	{
	class DependencyGraph;
	class TargetLoader;

	class SpiderDefault:public Spider
		{
		public:
			SpiderDefault(const std::map<Stringkey,const TargetLoader*>&&,DependencyGraph&)=delete;

			explicit
			SpiderDefault(const std::map<Stringkey,const TargetLoader*>& loaders
				,DependencyGraph& targets);

			SpiderDefault& scanFile(const char* filename,const char* in_dir);
			SpiderDefault& run();
			const char* targetPrefixGet() const noexcept;
			const char* sourcePrefixGet() const noexcept;

		private:
			const std::map<Stringkey,const TargetLoader*>& r_loaders;
			DependencyGraph& r_targets;

			typedef Stringkey VisitedKey;
			std::stack< Twins<std::string> > m_files_to_scan;
			std::set<VisitedKey> m_files_visited;
		};
	}

#endif
