//@	{
//@	 "targets":[{"name":"targetdirectorycompiler.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetdirectorycompiler.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETDIRECTORYCOMPILER_HPP
#define MAIKE_TARGETDIRECTORYCOMPILER_HPP

#include "targetcompiler.hpp"
#include <string>

namespace Maike
	{
	class Invoker;
	class Target;

	class TargetDirectoryCompiler:public TargetCompiler
		{
		public:
			TargetDirectoryCompiler& directoryTargetsSet(const char* dirname);
            void compile(Target& target,Invoker& invoker) const;
            bool upToDate(const Target& target,Invoker& invoker) const;

		private:
			std::string m_dir_targets;
		};
	}

#endif

