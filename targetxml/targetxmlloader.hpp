//@	{
//@	 "targets":[{"name":"targetxmlloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetxmlloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETXMLLOADER_HPP
#define TARGETXMLLOADER_HPP

#include "../target_loader.hpp"
#include "../command.hpp"

namespace Maike
	{
	class ResourceObject;
	class ParameterSetDumpable;

	class PRIVATE TargetXMLLoader:public Target_Loader
		{
		public:
			explicit TargetXMLLoader(ParameterSetDumpable&& sysvars)=delete;

			explicit TargetXMLLoader(const ParameterSetDumpable& sysvars);

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const;

			void configClear();

			TargetXMLLoader& configAppendDefault();

			TargetXMLLoader& configAppend(const ResourceObject& config);

			void configDump(ResourceObject& config) const;

		private:
			Command m_filter;
			const ParameterSetDumpable& r_sysvars;
		};
	}

#endif
