//@	{
//@	  "targets":[{"name":"string_template.hpp","type":"include"}]
//@	  ,"dependencies_extra":[{"ref":"string_template.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_STRINGTEMPLATE_HPP
#define MAIKE_STRINGTEMPLATE_HPP

#include <string>

namespace Maike
{
	namespace string_template_detail
	{
		std::string substitute(std::string (*do_lookup)(void const* container, std::string_view key),
		                       void const* container,
		                       char const* tempalte_string);
	}

	template<class AssociativeContainer>
	std::string substitute(char const* template_string, AssociativeContainer const& container)
	{
		return string_template_detail::substitute(
		    [](void const* container, std::string_view key) {
			   auto const& self = *reinterpret_cast<AssociativeContainer const*>(container);
			   auto const i = self.find(key);
			   if(i == self.end()) { return std::string{}; }
			   return i->second;
		   }, &container, template_string);
	}
}

#endif