//@	{
//@  "targets":[{"name":"targetpythoninterpreter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetpythoninterpreter.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETPYTHONINTERPRETER_HPP
#define MAIKE_TARGETPYTHONINTERPRETER_HPP

#include "command.hpp"

namespace Maike
	{
	class ResourceObject;
	class TargetPythonInterpreter
		{
		public:
			explicit TargetPythonInterpreter(const ParameterSet& sysvars);

			explicit TargetPythonInterpreter(ParameterSet&& sysvars)=delete;

			TargetPythonInterpreter(const ResourceObject& pythonoptions
				,ParameterSet&& sysvars)=delete;

			void run(const char* script,Twins<const char* const*> args) const;

			void configClear();

			TargetPythonInterpreter& configAppend(const ResourceObject& pythonoptions);

			void configDump(ResourceObject& pythonoptions) const;

		private:
			Command m_interpreter;
			const ParameterSet& r_sysvars;
		};
	}

#endif
