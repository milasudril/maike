//@	{
//@	 "targets":[{"name":"targetcploader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcploader.o","rel":"implementation"}]
//@	}

#ifndef TARGETCPLOADER_HPP
#define TARGETCPLOADER_HPP

#include "../target_loader.hpp"
#include "../command.hpp"

namespace Maike
	{
	class ResourceObject;

	class PRIVATE TargetCPLoader:public Target_Loader
		{
		public:
			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const override;

			void configClear();

			void dependenciesExtraGet(const char*,const char*
				,const char*,ResourceObject::Reader
				,DependencyBuffer&) const override
				{}

			void dependenciesGet(const char*,const char*
				,const char*,ResourceObject::Reader
				,DependencyBuffer&) const override
				{}

			Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root
				,size_t id,size_t line_count) const override;

		private:
		};
	}

#endif
