//@	{"targets":[{"name":"wand2maike","type":"application"}]}

#include "targetcxxpptokenizer.hpp"
#include "fileutils.hpp"

using namespace Maike;

int main(int argc,char** argv)
	{
	if(argc<2)
		{
		printf("Usage: wand2maike files ...\n\n");
		return 0;
		}

	FileUtils::mkdir("maike");

	}
