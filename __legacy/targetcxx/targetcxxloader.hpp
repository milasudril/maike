//@	{
//@	 "targets":[{"name":"targetcxxloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxloader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXLOADER_HPP
#define MAIKE_TARGETCXXLOADER_HPP

#include "../target_loader.hpp"
#include "../visibility.hpp"

namespace Maike
{
	class TargetCxxOptions;
	class TargetCxxCompiler;

	class PRIVATE TargetCxxLoader: public Target_Loader
	{
	public:
		explicit TargetCxxLoader(const TargetCxxOptions& options, const TargetCxxCompiler& compiler);

		template<class... T>
		explicit TargetCxxLoader(TargetCxxOptions&& options, T...) = delete;

		void targetsLoad(const char* name_src,
		                 const char* in_dir,
		                 Spider& spider,
		                 DependencyGraph& graph,
		                 Target_FactoryDelegator& factory) const;

		void dependenciesExtraGet(const char* name_src,
		                          const char* in_dir,
		                          const char* root,
		                          ResourceObject::Reader rc_reader,
		                          DependencyBuffer& deps) const;

		void dependenciesGet(const char* name_src,
		                     const char* in_dir,
		                     const char* root,
		                     ResourceObject::Reader rc_reader,
		                     DependencyBuffer& deps) const;

		Handle<Target> targetCreate(const ResourceObject& obj,
		                            const char* name_src,
		                            const char* in_dir,
		                            const char* root,
		                            size_t id,
		                            size_t line_count) const;

	private:
		const TargetCxxOptions& r_options;
		const TargetCxxCompiler& r_compiler;
	};
}

#endif
