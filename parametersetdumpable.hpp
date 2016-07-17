//@	{"targets":[{"name":"parametersetdumpable.hpp","type":"include"}]}

#ifndef MAIKE_PARAMETERSETDUMPABLE_HPP
#define MAIKE_PARAMETERSETDUMPABLE_HPP

#include "parameterset.hpp"

namespace Maike
	{
	class ResourceObject;
	class ParameterSetDumpable:public ParameterSet
		{
		public:
			virtual void configDump(ResourceObject& targetinfo) const=0;
		};
	}

#endif
