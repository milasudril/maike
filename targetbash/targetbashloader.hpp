//@	{
//@	 "targets":[{"name":"targetbashloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetbashloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETBASHLOADER_HPP
#define TARGETBASHLOADER_HPP

#include "../target_loader.hpp"

namespace Maike
	{
	class TargetBashInterpreter;

	class PRIVATE TargetBashLoader:public Target_Loader
		{
		public:
			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const override;

			void dependenciesExtraGet(const char* name_src,const char* in_dir
				,const char* root,ResourceObject::Reader rc_reader
				,DependencyBuffer& deps) const override;

			void dependenciesGet(const char*,const char*
				,const char*,ResourceObject::Reader
				,DependencyBuffer&) const override
				{}

			explicit TargetBashLoader(const TargetBashInterpreter& intptret);

			TargetBashLoader(TargetBashInterpreter&&)=delete;

			Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root
				,size_t id,size_t line_count) const override;

		private:
			const TargetBashInterpreter& r_intpret;
		};
	}

#endif
