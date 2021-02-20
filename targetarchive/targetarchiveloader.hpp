//@	{
//@	 "targets":[{"name":"targetarchiveloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetarchiveloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETARCHIVELOADER_HPP
#define TARGETARCHIVELOADER_HPP

#include "../target_loader.hpp"

namespace Maike
	{
	class TargetArchiveCompiler;
	class PRIVATE TargetArchiveLoader:public Target_Loader
		{
		public:
			explicit TargetArchiveLoader(const TargetArchiveCompiler& compiler);
			explicit TargetArchiveLoader(TargetArchiveCompiler&& compiler)=delete;

			void targetsLoad(const char* name_src,const char* in_dir
				,Spider& spider,DependencyGraph& graph
				,Target_FactoryDelegator& factory) const override;

			void dependenciesExtraGet(const char*,const char*
				,const char*,ResourceObject::Reader
				,DependencyBuffer& ) const override
				{}

			void dependenciesGet(const char* name_src,const char* in_dir
				,const char* root,ResourceObject::Reader rc_reader
				,DependencyBuffer& deps) const override;

			Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root
				,size_t id,size_t line_count) const override;

		private:
			const TargetArchiveCompiler& r_compiler;
		};
	}

#endif
