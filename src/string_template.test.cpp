//@	{
//@	  "targets":[{"name":"string_template.test","type":"application","autorun":1}]
//@	}

#undef NDEBUG

#include "./string_template.hpp"

#include <map>
#include <cassert>

namespace Testcases
{
	void maikeStringTemplateSubstitute()
	{
		std::map<std::string, std::string, std::less<>> substitutes{{"a variable", "some value"}};

		auto result = Maike::substitute(
		   R"test(This is {%a variable%}. {%Non existing value%}. Escape {\%this is not a variable}. A backslash \\. %a variable)test",
		   substitutes);
		assert(
		   result
		   == "This is {some value}. {}. Escape {%this is not a variable}. A backslash \\. some value");
	}
}

int main()
{
	Testcases::maikeStringTemplateSubstitute();
	return 0;
}