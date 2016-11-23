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
	class PRIVATE Options
		{
		public:
			struct PRIVATE Option
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
				,Stringkey("configdump")
				,Stringkey("no-sysvars")
				,Stringkey("hooks-load")
				,Stringkey("targets")
				,Stringkey("list-leaf-targets")
				,Stringkey("list-external-targets")
				,Stringkey("list-all-targets")
				,Stringkey("dump-database-json")
				,Stringkey("dump-graph-dot")
				,Stringkey("dump-graph-inv-dot")
				,Stringkey("dump-targets-tsv")
				,Stringkey("load-path")
				,Stringkey("configclean")
				,Stringkey("clean")
				,Stringkey("remove-orphans")
				,Stringkey("about")
				> OptionMap;

			Options(Twins<const char* const*> args);

			void printHelp(DataSink&& sink) const
				{printHelpImpl(sink);}

			void printHelp(DataSink& sink) const
				{printHelpImpl(sink);}

			template<Stringkey::HashValue key>
			const std::vector<std::string>* get() const noexcept
				{return m_options.get<key>().values;}

		private:
			OptionMap m_options;
			void printHelpImpl(DataSink& sink) const;

			std::vector<std::string> m_data[OptionMap::size()];
		};
	}

#endif
