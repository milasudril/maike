//@	{
//@	 "targets":[{"name":"command.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_COMMAND_HPP
#define MAIKE_COMMAND_HPP

#include "pipe.hpp"
#include <vector>
#include <string>
#include <map>

namespace Maike
	{
	class Variant;
	class DataSink;
	class Stringkey;
	class ResourceObject;

	class Command
		{
		public:
			Pipe execute(unsigned int redirection) const;

			Pipe execute(unsigned int redirection
				,const std::map<Stringkey,std::string>& substitutes) const;

			Command();
			Command(const ResourceObject& cmd);

			Command& nameSet(const char* name);
			Command& argumentAppend(const char* arg);


		private:
			std::string m_name;
			std::vector< std::string > m_args;
		};
	}

#endif
