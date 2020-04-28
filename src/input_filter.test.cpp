//@	{
//@	  "targets":[{"name":"input_filter.test","type":"application", "autorun":1}]
//@	}

#include "./input_filter.hpp"

namespace Testcases
{
	void maikeInputFilterDefault()
	{
		Maike::InputFilter f;
		assert(f.match(".foobar"));
		assert(f.match("__foobar"));
		assert(!f.match("foo.bar"));
		assert(!f.match("foo__bar"));
	}
}

int main()
{
	Testcases::maikeInputFilterDefault();
}