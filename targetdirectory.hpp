//@	{
//@	"targets":[{"name":"targetdirectory.hpp","type":"include"}]
//@	"dependencies_extra":[{"ref":"targetdirectory.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETDIRECTORY_HPP
#define MAIKE_TARGETDIRECTORY_HPP

#include "targetbase.hpp"

namespace Maike
	{
	class TargetDirectory:public TargetBase
		{
		public:
			TargetDirectory(const char* name,const char* in_dir,size_t id):
				TargetBase(name,name,in_dir,id)
				{}

			void compile(Twins<const Dependency*> dependency_list
				,Invoker& invoker);

			bool upToDate(Twins<const Dependency*> dependency_list
				,Invoker& invoker) const;
		};
	}

#endif
