//@	{
//@	 "targets":[{"name":"targetpythonfactory.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetpythonfactory.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETPYTHONFACTORY_HPP
#define MAIKE_TARGETPYTHONFACTORY_HPP

#include "../target_factory.hpp"
#include "targetpython.hpp"

namespace Maike
	{
	class TargetPythonInterpreter;

	class PRIVATE TargetPythonFactory:public Target_Factory
		{
		public:
			explicit TargetPythonFactory(const TargetPythonInterpreter& intptret);
			TargetPythonFactory(TargetPythonInterpreter&&)=delete;

			Handle<Target> targetCreate(const ResourceObject& obj
				,const char* name_src,const char* in_dir,const char* root	
				,size_t id,size_t line_count) const;

		private:
			const TargetPythonInterpreter& r_intpret;
		};
	}

#endif
