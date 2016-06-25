//@	{
//@	"targets":[{"name":"targetdirectory.hpp","type":"include"}]
//@	"dependencies_extra":[{"ref":"targetdirectory.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETDIRECTORY_HPP
#define MAIKE_TARGETDIRECTORY_HPP

#include "targetbase.hpp"

namespace Maike
	{
	class TargetDirectoryCompiler;

	class TargetDirectory:public TargetBase
		{
		public:
			explicit TargetDirectory(const char* name,const char* in_dir
				,size_t id,TargetDirectoryCompiler&& compiler)=delete;

			explicit TargetDirectory(const char* name,const char* in_dir
				,size_t id,const TargetDirectoryCompiler& compiler):
				TargetBase(name,name,in_dir,id),r_compiler(compiler)
				{}

			void compile(Twins<const Dependency*> dependency_list
				,Invoker& invoker);

			bool upToDate(Twins<const Dependency*> dependency_list
				,Invoker& invoker) const;

		private:
			const TargetDirectoryCompiler& r_compiler;
		};
	}

#endif
