//@	{
//@	 "targets":[{"name":"systemtargetinfo.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"systemtargetinfo.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SYSTEMTARGETINFO_HPP
#define MAIKE_SYSTEMTARGETINFO_HPP

#include "parameterset.hpp"
#include <string>
#include <map>
#include <vector>

namespace Maike
	{
	class ResourceObject;
	class Variant;

	class SystemTargetInfo:public ParameterSet
		{
		public:
			SystemTargetInfo(const ResourceObject& sysvars);
			~SystemTargetInfo();

			void parameterGet(const Stringkey& key
				,ParameterProcessor&& proc) const;

			Variant variableGet(const Stringkey& key) const noexcept;

		private:
			std::map<Stringkey,Variant> m_sysvars;
			std::map<Stringkey,std::string> m_strings;
		};
	}

#endif // MAIKE_SYSTEMINFO_HPP
