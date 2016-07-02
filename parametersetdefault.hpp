//@	{"targets":[{"name":parametersetdefault.hpp","type":"include"}]}

#ifndef MAIKE_PARAMETERSETDEFAULT_HPP
#define MAIKE_PARAMETERSETDEFAULT_HPP

#include "parameterset.hpp"

#include <vector>
#include <string>
#include <map>

namespace Maike
	{
	class ParameterSetDefault:public ParameterSet
		{
		public:
			ParameterSetDefault();
			~ParameterSetDefault();

			ParameterSetDefault& parameterSet(const Stringkey& key
				,Twins<const char* const*> strings);
			void parameterGet(const Stringkey& key,ParameterProcessor&& proc) const;

		private:
			std::map< Stringkey, std::vector<std::string> > m_values;
		};
	}

#endif // MAIKE_PARAMETERSETDEFAULT_HPP
