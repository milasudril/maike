//@	{
//@	"targets":[{"name":"targetdirectoryloader.hpp","type":"include"}]
//@	"dependencies_extra":[{"ref":targetdirectoryloader.o","rel":"implementation}]
//@	}

#ifndef MAIKE_TARGETDIRECTORYLOADER_HPP
#define MAIKE_TARGETDIRECTORYLOADER_HPP

#include "targetloader.hpp"
#include "stringkey.hpp"
#include <set>

namespace Maike
	{
	class TargetDirectoryCompiler;
	class TargetDirectoryLoader:public TargetLoader
		{
		public:
			TargetDirectoryLoader(TargetDirectoryCompiler&& compiler)=delete;

			explicit TargetDirectoryLoader(const TargetDirectoryCompiler& compiler);

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph) const;

			TargetDirectoryLoader& pathRefuse(const Stringkey& key);
			TargetDirectoryLoader& pathAllow(const Stringkey& key);

		private:
			const TargetDirectoryCompiler& r_compiler;
			std::set<Stringkey> m_ignore;
			bool m_recursive;
		};
	}

#endif
