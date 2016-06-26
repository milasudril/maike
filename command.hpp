//@	{
//@	 "targets":[{"name":"command.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_COMMAND_HPP
#define MAIKE_COMMAND_HPP

#include <vector>
#include <string>
#include <map>

namespace Maike
	{
	class Invoker;
	class Variant;
	class DataSink;
	class Stringkey;

	class Command
		{
		public:
			void execute(Invoker& invoker,DataSink& standard_output
				,DataSink& standard_error);

			void execute(Invoker& invoker,DataSink& standard_output
				,DataSink& standard_error
				,const std::map<Stringkey,std::string>& substitutes);

			Command& nameSet(const char* name);
			Command& argumentAppend(const char* arg);


		private:
			std::string m_name;
			std::vector< std::string > m_args;
		};
	}

#endif
