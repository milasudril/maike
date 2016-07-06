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

static int  helpPrint(const Maike::Options& opts,const std::vector<std::string>& help)
	{
	if(help.size()==0)
		{
		opts.printHelp(DataSinkStd::standard_output);
		return 0;
		}
	opts.printHelp(FileOut{help.begin()->c_str()});
	return 0;
	}

int main(int argc,char** argv)
	{
	try
		{
		Maike::Options opts{Maike::Twins<const char* const*>(argv,argv+argc)};
			{
			auto x=opts.get<Stringkey("help")>();
			if(x!=nullptr)
				{return helpPrint(opts,*x);}
			}

		}
	catch(const ErrorMessage& message)
		{
		fprintf(stderr,"%s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
