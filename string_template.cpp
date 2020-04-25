//@	{
//@	  "targets":[{"name":"string_template.o","type":"object"}]
//@	}

#include "./string_template.hpp"

namespace
{
	auto make_string_view(char const* begin, char const* end)
	{
		return std::string_view{begin, static_cast<size_t>(end - begin)};
	}
}

std::string Maike::string_template_detail::substitute(
   std::string (*do_lookup)(void const* container, std::string_view key),
   void const* container,
   char const* template_string)
{
	std::string ret;
	enum class State : int
	{
		Normal,
		Placeholder,
		Escape
	};
	auto state = State::Normal;
	char const* placeholder_begin = nullptr;
	while(true)
	{
		auto ch_in = *template_string;
		++template_string;
		switch(state)
		{
			case State::Normal:
				switch(ch_in)
				{
					case '\0': return ret;

					case '%':
						state = State::Placeholder;
						placeholder_begin = template_string;
						break;

					case '\\': state = State::Escape; break;

					default: ret += ch_in; break;
				}
				break;

			case State::Placeholder:
				switch(ch_in)
				{
					case '\0':
						ret += do_lookup(container, make_string_view(placeholder_begin, template_string - 1));
						return ret;

					case '%':
						state = State::Normal;
						ret += do_lookup(container, make_string_view(placeholder_begin, template_string - 1));
						break;
				}
				break;

			case State::Escape:
				switch(ch_in)
				{
					case '\0': ret += '\\'; return ret;

					default:
						ret += ch_in;
						state = State::Normal;
						break;
				}
		}
	}
}
