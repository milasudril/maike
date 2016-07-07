//@	{
//@	 "targets":[{"name":"targetdirectoryloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetdirectoryloader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETDIRECTORYLOADER_HPP
#define MAIKE_TARGETDIRECTORYLOADER_HPP

#include "targetloader.hpp"
#include "stringkey.hpp"
#include <set>

namespace Maike
	{
	class ResourceObject;
	class TargetDirectoryLoader:public TargetLoader
		{
		public:
			TargetDirectoryLoader(const ResourceObject& directoryloader);

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,const ExpressionEvaluator& evaluator) const;

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

			TargetDirectoryLoader& pathReject(const Stringkey& key);
			TargetDirectoryLoader& pathAccept(const Stringkey& key);

		private:
			std::set<Stringkey> m_ignore;
			bool m_recursive;
		};
	}

#endif
