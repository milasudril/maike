//@	{
//@	 "targets":[{"name":"targetdirectoryloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetdirectoryloader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETDIRECTORYLOADER_HPP
#define MAIKE_TARGETDIRECTORYLOADER_HPP

#include "target_loader.hpp"
#include "stringkey.hpp"
#include "visibility.hpp"
#include <map>
#include <string>

namespace Maike
	{
	class ResourceObject;
	class PRIVATE TargetDirectoryLoader:public Target_Loader
		{
		public:
			TargetDirectoryLoader();

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

			TargetDirectoryLoader& pathReject(const char* name);
			TargetDirectoryLoader& pathAccept(const char* name);

			void configClear();
			TargetDirectoryLoader& configAppendDefault();
			TargetDirectoryLoader& configAppend(const ResourceObject& directoryoptions);
			void configDump(ResourceObject& directoryoptions) const;

		private:
			std::map<Stringkey,std::string> m_ignore;
			bool m_recursive;
		};
	}

#endif
