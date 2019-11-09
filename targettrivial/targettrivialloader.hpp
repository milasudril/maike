//@	{
//@	 "targets":[{"name":"targettrivialloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targettrivialloader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETTRIVIALLOADER_HPP
#define MAIKE_TARGETTRIVIALLOADER_HPP

#include "targettrivialoptions.hpp"

#include "../target_loader.hpp"

namespace Maike
	{
	class ResourceObject;

	class PRIVATE TargetTrivialLoader:public Target_Loader
		{
		public:
			explicit TargetTrivialLoader(const TargetTrivialOptions& opts):r_opts{opts}{}

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const override;

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
			const TargetTrivialOptions& r_opts;
		};
	}

#endif
