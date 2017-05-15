//@	{
//@	 "targets":[{"name":"targetoctavefactory.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetoctavefactory.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETOCTAVEFACTORY_HPP
#define MAIKE_TARGETOCTAVEFACTORY_HPP

#include "../target_factory.hpp"
#include "targetoctave.hpp"

namespace Maike
	{
	class TargetOctaveInterpreter;

	class PRIVATE TargetOctaveFactory:public Target_Factory
		{
		public:
			explicit TargetOctaveFactory(const TargetOctaveInterpreter& intptret);
			TargetOctaveFactory(TargetOctaveInterpreter&&)=delete;

			Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root	
				,size_t id,size_t line_count) const;

		private:
			const TargetOctaveInterpreter& r_intpret;
		};
	}

#endif
