//@	{
//@  "targets":[{"name":"targetpythoninterpreter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetpythoninterpreter.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETPYTHONINTERPRETER_HPP
#define MAIKE_TARGETPYTHONINTERPRETER_HPP

#include "../command.hpp"

namespace Maike
{
	class ResourceObject;
	class ParameterSetDumpable;
	class PRIVATE TargetPythonInterpreter
	{
	public:
		explicit TargetPythonInterpreter(const ParameterSetDumpable& sysvars);

		explicit TargetPythonInterpreter(ParameterSetDumpable&& sysvars) = delete;

		TargetPythonInterpreter(const ResourceObject& pythonoptions, ParameterSet&& sysvars) = delete;

		int run(const char* script, Twins<const char* const*> args) const;

		void configClear();

		TargetPythonInterpreter& configAppendDefault();

		TargetPythonInterpreter& configAppend(const ResourceObject& pythonoptions);

		void configDump(ResourceObject& pythonoptions) const;

		const char* executable() const noexcept;

	private:
		Command m_interpreter;
		const ParameterSetDumpable& r_sysvars;
	};
}

#endif
