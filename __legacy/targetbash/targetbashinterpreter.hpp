//@	{
//@  "targets":[{"name":"targetbashinterpreter.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetbashinterpreter.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETBASHNINTERPRETER_HPP
#define MAIKE_TARGETBASHINTERPRETER_HPP

#include "../command.hpp"

namespace Maike
{
	class ResourceObject;
	class ParameterSetDumpable;
	class PRIVATE TargetBashInterpreter
	{
	public:
		explicit TargetBashInterpreter(const ParameterSetDumpable& sysvars);

		explicit TargetBashInterpreter(ParameterSetDumpable&& sysvars) = delete;

		TargetBashInterpreter(const ResourceObject& pythonoptions, ParameterSet&& sysvars) = delete;

		int run(const char* script, Twins<const char* const*> args) const;

		void configClear();

		TargetBashInterpreter& configAppendDefault();

		TargetBashInterpreter& configAppend(const ResourceObject& pythonoptions);

		void configDump(ResourceObject& pythonoptions) const;

		const char* executable() const noexcept;

	private:
		Command m_interpreter;
		const ParameterSetDumpable& r_sysvars;
	};
}

#endif
