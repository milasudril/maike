//@	{
//@	 "targets":[{"name":"systemtargetinfo.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"systemtargetinfo.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_SYSTEMTARGETINFO_HPP
#define MAIKE_SYSTEMTARGETINFO_HPP

#include "parametersetdumpable.hpp"
#include "visibility.hpp"
#include <string>
#include <map>
#include <vector>

namespace Maike
	{
	class Variant;

	class PRIVATE SystemTargetInfo:public ParameterSetDumpable
		{
		public:
			SystemTargetInfo();
			~SystemTargetInfo() noexcept;

			void parameterGet(const Stringkey& key
				,ParameterProcessor&& proc) const;

			Variant variableGet(const Stringkey& key) const noexcept;

			void clear();

			SystemTargetInfo& configAppend(const ResourceObject& targetinfo);
			void configDump(ResourceObject& targetinfo) const;

			SystemTargetInfo& sysvarsLoad();
			SystemTargetInfo& configAppendDefault();

		private:
			std::map<Stringkey,Variant> m_sysvars;
			std::map<Stringkey,std::string> m_strings;
			std::map<Stringkey,std::string> m_varnames;
		};
	}

#endif // MAIKE_SYSTEMINFO_HPP
