//@	{"targets":[{"name":"pathutils-test","type":"application"}]}

#include "pathutils.hpp"
#include "errormessage.hpp"
#include <cstdio>

using namespace Maike;

int main()
	{
	try
		{
		auto a=directoryNormalize("/foo/bar/../bajs");
		printf("%s\n",a.c_str());

		a=directoryNormalize("foo/bar/../../kaka");
		printf("%s\n",a.c_str());

		a=directoryNormalize("foo/bar/../../../../kaka");
		printf("%s\n",a.c_str());
		
		a=directoryNormalize("/foo/bar/../../../../kaka");
		printf("%s\n",a.c_str());
		}
	catch(const ErrorMessage& msg)
		{
		fprintf(stderr,"%s\n",msg.messageGet());
		}

	return 0;
	}