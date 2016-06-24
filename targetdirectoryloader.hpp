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
	class TargetDirectoryLoader:public TargetLoader
		{
		public:
			TargetDirectoryLoader();
			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph) const;

		private:
			std::set<Stringkey> m_ignore;
			bool m_recursive;
		};
	}

#endif
