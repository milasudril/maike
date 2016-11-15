//@	{
//@	 "targets":[{"name":"targetarchivefactory.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetarchivefactory.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETARCHIVEFACTORY_HPP
#define MAIKE_TARGETARCHIVEFACTORY_HPP

#include "../target_factory.hpp"
#include "targetarchive.hpp"

namespace Maike
	{
	class TargetArchiveCompiler;

	class PRIVATE TargetArchiveFactory:public Target_Factory
		{
		public:
			explicit TargetArchiveFactory(const TargetArchiveCompiler& intptret);
			TargetArchiveFactory(TargetArchiveCompiler&&)=delete;

			Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root	
				,size_t id,size_t line_count) const;

		private:
			const TargetArchiveCompiler& r_intpret;
		};
	}

#endif
