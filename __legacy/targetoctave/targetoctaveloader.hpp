//@	{
//@	 "targets":[{"name":"targetoctaveloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetoctaveloader.o","rel":"implementation"}]
//@	}

#ifndef TARGETOCTAVELOADER_HPP
#define TARGETOCTAVELOADER_HPP

#include "../target_loader.hpp"

namespace Maike
{
	class TargetOctaveInterpreter;

	class PRIVATE TargetOctaveLoader: public Target_Loader
	{
	public:
		void targetsLoad(const char* name_src,
		                 const char* in_dir,
		                 Spider& spider,
		                 DependencyGraph& graph,
		                 Target_FactoryDelegator& factory) const;

		void dependenciesExtraGet(const char* name_src,
		                          const char* in_dir,
		                          const char* root,
		                          ResourceObject::Reader rc_reader,
		                          DependencyBuffer& deps_out) const;

		void dependenciesGet(
		   const char*, const char*, const char*, ResourceObject::Reader, DependencyBuffer&) const
		{
		}

		explicit TargetOctaveLoader(const TargetOctaveInterpreter& intptret);

		Handle<Target> targetCreate(const ResourceObject& obj,
		                            const char* name_src,
		                            const char* in_dir,
		                            const char* root,
		                            size_t id,
		                            size_t line_count) const;

	private:
		const TargetOctaveInterpreter& r_intpret;
	};
}

#endif
