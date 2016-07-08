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
			TargetPythonInterpreter(const ResourceObject& pythonoptions);
			void run(const char* script,Twins<const char* const*> args) const;

		private:
			Command m_interpreter;
		};
	}

#endif