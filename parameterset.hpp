//@	{"targets":[{"name":"parameterset.hpp","type":"include"}]}

#ifndef MAIKE_PARAMETERSET_HPP
#define MAIKE_PARAMETERSET_HPP

#include "twins.hpp"

namespace Maike
	{
	class Stringkey;
	class ParameterSet
		{
		public:
			class ParameterProcessor
				{
				public:
					virtual void operator()(const char* element)=0;
				};

			virtual void parameterGet(const Stringkey& key
				,ParameterProcessor&& proc) const=0;

		protected:
			~ParameterSet()=default;
		};
	}

#endif
