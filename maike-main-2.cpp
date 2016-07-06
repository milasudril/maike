//@	[
//@		["windows"
//@			,{"targets":[{"name":"maike2.exe","type":"application"}]}]
//@		,{"targets":[{"name":"maike2","type":"application"}]}
//@	]

#include "errormessage.hpp"
#include "options.hpp"
#include "datasinkstd.hpp"
#include "fileout.hpp"
#include <cstdio>

using namespace Maike;

int main(int argc,char** argv)
	{
	try
		{
		Maike::Options opts{Maike::Twins<const char* const*>(argv,argv+argc)};
			{
			auto x=opts.get<Stringkey("help")>();
			if(x!=nullptr)
				{
				if(x->size()==0)
					{
					opts.printHelp(DataSinkStd::standard_output);
					return 0;
					}
				opts.printHelp(FileOut{x->begin()->c_str()});
				return 0;
				}
			}

		}
	catch(const ErrorMessage& message)
		{
		fprintf(stderr,"%s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
