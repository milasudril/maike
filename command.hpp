//@	{
//@	 "targets":[{"name":"command.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_COMMAND_HPP
#define MAIKE_COMMAND_HPP

#include "pipe.hpp"
#include "twins.hpp"
#include <vector>
#include <string>

namespace Maike
	{
	class Variant;
	class Stringkey;
	class ResourceObject;
	class ParameterSet;

	class Command
		{
		public:
			Pipe execute(unsigned int redirection) const;

			Pipe execute(unsigned int redirection
				,Twins<const ParameterSet* const*> substitutes) const;

			Command();
			Command(const ResourceObject& cmd);

			Command& nameSet(const char* name);
			Command& argumentAppend(const char* arg);
			Command& argumentsClear() noexcept;

			void configDump(ResourceObject& cmd) const;


		private:
			std::string m_name;
			std::vector< std::string > m_args;
		};
	}

#endif
