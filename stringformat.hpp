//@	{
//@	 "targets":[{"name":"stringformat.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"stringformat.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_STRINGFORMAT_HPP
#define MAIKE_STRINGFORMAT_HPP

#include "variant.hpp"
#include "twins.hpp"
#include <initializer_list>
#include <cstddef>

namespace Maike
	{
	PRIVATE void format(const Twins<char*>& buffer,const char* format_string
		,const Twins<const Variant*>& args);

	inline void format(const Twins<char*>& buffer,const char* format_string
		,const std::initializer_list<Variant>& args)
		{format(buffer,format_string,Twins<const Variant*>(args.begin(),args.end()));}

	}

#endif
