//@	{"targets":[{"name":"pathutils_test","type":"application"}]}

#include "pathutils.hpp"

int main()
	{
	const char* teststring="./subsystem/foo/../../generic_stuff.hpp";

	printf("%s\n",Maike::directoryNormalize(teststring).c_str());
	}
