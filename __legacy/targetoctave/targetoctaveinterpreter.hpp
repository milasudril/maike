//@	{
//@  "targets":[{"name":"targetoctaveinterpreter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetoctaveinterpreter.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETOCTAVEINTERPRETER_HPP
#define MAIKE_TARGETOCTAVEINTERPRETER_HPP

#include "../command.hpp"

namespace Maike
	{
	class ResourceObject;
	class ParameterSetDumpable;
	class PRIVATE TargetOctaveInterpreter
		{
		public:
			explicit TargetOctaveInterpreter(const ParameterSetDumpable& sysvars);

			explicit TargetOctaveInterpreter(ParameterSetDumpable&& sysvars)=delete;

			TargetOctaveInterpreter(const ResourceObject& octaveoptions
				,ParameterSet&& sysvars)=delete;

			int run(const char* script,Twins<const char* const*> args) const;

			void configClear();

			TargetOctaveInterpreter& configAppendDefault();

			TargetOctaveInterpreter& configAppend(const ResourceObject& octaveoptions);

			void configDump(ResourceObject& octaveoptions) const;

			const char* executable() const noexcept
				{return m_interpreter.nameGet();}

		private:
			Command m_interpreter;
			const ParameterSetDumpable& r_sysvars;
		};
	}

#endif
