//@	{
//@	  "targets":[{"name":"string_template.hpp","type":"include"}]
//@	  ,"dependencies_extra":[{"ref":"string_template.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_TEMPLATESTRING_HPP
#define MAIKE_TEMPLATESTRING_HPP

#include <string>

namespace Maike
{
	namespace string_template_detail
	{
		std::string substitute(char const* tempalte_string,
		                       void const* container,
		                       std::string (*do_lookup)(void const* container, std::string_view key));
	}

	template<class AssociativeContainer>
	std::string substitute(char const* template_string, AssociativeContainer const& container)
	{
		return string_template_detail::substitute(
		   template_string, &container, [](void const* container, std::string_view key) {
			   auto const& self = *reinterpret_cast<AssociativeContainer const*>(container);
			   auto const i = self.find(key);
			   if(i == self.end()) { return std::string{}; }
			   return i->second;
		   });
	}
}

#endif