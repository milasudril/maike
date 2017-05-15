//@	{
//@	 "targets":[{"name":"command.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_COMMAND_HPP
#define MAIKE_COMMAND_HPP

#include "pipe.hpp"
#include "twins.hpp"
#include "visibility.hpp"
#include <vector>
#include <string>

namespace Maike
	{
	class Variant;
	class Stringkey;
	class ResourceObject;
	class ParameterSet;

	class PRIVATE Command
		{
		public:
			Pipe execute(unsigned int redirection) const;

			Pipe execute(unsigned int redirection
				,Twins<const ParameterSet* const*> substitutes) const;

			Command();
			Command(const ResourceObject& cmd);

			Command& nameSet(const char* name);
			Command& argumentAppend(const char* arg);
			void argumentsClear() noexcept;

			void configDump(ResourceObject& cmd) const;

			operator bool() const noexcept
				{return m_name.size()!=0;}

			const char* nameGet() const noexcept
				{return m_name.c_str();}

		private:
			std::string m_name;
			std::vector< std::string > m_args;
		};
	}

#endif
