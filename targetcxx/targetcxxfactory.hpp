//@	{
//@	 "targets":[{"name":"targetcxxfactory.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxxfactory.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXXFACTORY_HPP
#define MAIKE_TARGETCXXFACTORY_HPP

#include "../target_factory.hpp"
#include "targetcxx.hpp"

namespace Maike
	{
	class TargetCxxCompiler;

	class PRIVATE TargetCxxFactory:public Target_Factory
		{
		public:
			explicit TargetCxxFactory(const TargetCxxCompiler& compiler);
			TargetCxxFactory(TargetCxxCompiler&&)=delete;

			Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,size_t id
				,size_t line_count) const;

		private:
			const TargetCxxCompiler& r_compiler;
		};
	}

#endif
