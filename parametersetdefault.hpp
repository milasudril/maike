//@	{
//@	 "targets":[{"name":"parametersetdefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"parametersetdefault.o","rel":"implementation"}]
//@	}

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

			void parameterGet(const Stringkey& key,ParameterProcessor&& proc) const;
			void clear() noexcept;

		private:
			std::map< Stringkey, std::vector<std::string> > m_values;
		};
	}

#endif // MAIKE_PARAMETERSETDEFAULT_HPP
