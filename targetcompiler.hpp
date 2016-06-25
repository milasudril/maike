//@{"targets":[{"name":"targetcompiler.hpp","type":"include"}]}

#ifndef MAIKE_TARGETCOMPILER_HPP
#define MAIKE_TARGETCOMPILER_HPP

namespace Maike
	{
	class TargetCompiler
		{
		public:
			virtual TargetCompiler& directoryTargetsSet(const char* dirname)=0;
		};
	}

#endif
