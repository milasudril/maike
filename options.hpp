//@	{
//@	 "targets":[{"name":"options.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"options.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_OPTIONS_HPP
#define MAIKE_OPTIONS_HPP

#include "twins.hpp"
#include "mapfixed.hpp"
#include "stringkey.hpp"
#include <string>
#include <vector>

namespace Maike
	{
	class DataSink;
	class Options
		{
		public:
			struct Option
				{
				const char* key;
				const char* description;
				const std::vector<std::string>* values;
				unsigned int argcount;
				unsigned int group;
				};

			typedef MapFixed<Stringkey::HashValue,Option
				,Stringkey("help")
				,Stringkey("version")
				,Stringkey("configfiles")
				,Stringkey("no-stdconfig")
				,Stringkey("no-sysvars")
				,Stringkey("targets")
				,Stringkey("list-leaf-targets")
				,Stringkey("list-external-targets")
				,Stringkey("list-all-targets")
				,Stringkey("dump-graph")
				,Stringkey("dump-target-usecount")
				,Stringkey("dump-timestat")> OptionMap;

			Options(Twins<const char* const*> args);

			void printHelp(DataSink&& sink) const
				{printHelp(std::move(sink));}

			void printHelp(DataSink& sink) const;

			template<Stringkey::HashValue key>
			const std::vector<std::string>* get() const noexcept
				{return m_options.get<key>().values;}

		private:
			OptionMap m_options;

			std::vector<std::string> m_data[OptionMap::size()];
		};
	}

#endif
