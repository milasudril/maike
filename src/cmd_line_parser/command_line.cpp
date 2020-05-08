//@	{
//@	  "targets":[{"name":"command_line.o","type":"object"}]
//@	}

#include "command_line.hpp"

namespace
{
	template<class Iter, class T, class Compare>
	Iter binaryFind(Iter first, Iter last, T const& val, Compare cmp)
	{
		first = std::lower_bound(first, last, val, cmp);
		return first != last && !cmp(val, *first) ? first : last;
	}
}

void Maike::CmdLineParser::detail::collect_options(char const* const* argv_begin,
                                                   char const* const* argv_end,
                                                   OptItem const* optitems_begin,
                                                   OptItem const* optitems_end,
                                                   void* tuple)
{
	std::for_each(argv_begin, argv_end, [optitems_begin, optitems_end, tuple](auto str) {
		auto const l = strlen(str);
		if(l == 0) { return; }

		auto const str_end = str + l;
		auto splitpoint = std::find(str, str_end, '=');

		auto x = binaryFind(optitems_begin,
		                    optitems_end,
		                    str,
		                    CompareOptItemsByName{static_cast<size_t>(splitpoint - str)});

		if(x == optitems_end)
		{ throw std::runtime_error{std::string{"Invalid command line option "} + str}; }

		if(splitpoint == str_end) { x->converter(tuple, ""); }
		else
		{
			x->converter(tuple, splitpoint + 1);
		}
	});
}