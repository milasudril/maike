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
	class ResourceObject;

	class Command
		{
		public:
			void execute(Invoker&& invoker,DataSink&& standard_output
				,DataSink&& standard_error) const
				{execute(std::move(invoker),std::move(standard_output),std::move(standard_error));}

			void execute(Invoker&& invoker,DataSink&& standard_output
				,DataSink&& standard_error
				,const std::map<Stringkey,std::string>& substitutes) const
				{execute(std::move(invoker),std::move(standard_output),std::move(standard_error),std::move(substitutes));}

			void execute(Invoker& invoker,DataSink& standard_output
				,DataSink& standard_error) const;

			void execute(Invoker& invoker,DataSink& standard_output
				,DataSink& standard_error
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
