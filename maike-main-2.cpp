//@	[
//@		["windows"
//@			,{"targets":[{"name":"maike2.exe","type":"application"}]}]
//@		,["1",{"targets":[{"name":"maike2","type":"application"}]}]
//@	]

#include "errormessage.hpp"
#include "options.hpp"
#include "datasinkstd.hpp"
#include "fileout.hpp"
#include "filein.hpp"
#include "resourceobject.hpp"
#include "session.hpp"
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

static int versionPrint(const std::vector<std::string>& version)
	{
	if(version.size()==0)
		{
	//	versionPrint(DataSinkStd::standard_output);
		return 0;
		}
//	versionPrint(FileOut{version.begin()->c_str()});
	return 0;
	}

static void configfilesLoad(Session& maike,const std::vector<std::string>& files)
	{
	auto ptr=files.data();
	auto ptr_end=ptr+files.size();
	while(ptr!=ptr_end)
		{
		ResourceObject rc{FileIn(ptr->c_str())};
		maike.configAppend(rc);
		++ptr;
		}
	}

static int configDump(const Maike::Session& maike,const std::vector<std::string>& configdump)
	{
	ResourceObject obj(ResourceObject::Type::OBJECT);
	maike.configDump(obj);
	if(configdump.size()==0)
		{
		obj.write(DataSinkStd::standard_output);
		return 0;
		}
	obj.write(FileOut{configdump.begin()->c_str()});
	return 0;
	}


int main(int argc,char** argv)
	{
	try
		{
		Session maike;
		Options opts{Twins<const char* const*>(argv,argv+argc)};
			{
			auto x=opts.get<Stringkey("help")>();
			if(x!=nullptr)
				{return helpPrint(opts,*x);}
			}

			{
			auto x=opts.get<Stringkey("version")>();
			if(x!=nullptr)
				{return versionPrint(*x);}
			}

		if(opts.get<Stringkey("no-sysvars")>()!=nullptr)
			{maike.configClear();}

			{
			auto x=opts.get<Stringkey("configfiles")>();
			if(x!=nullptr)
				{configfilesLoad(maike,*x);}
			}

			{
			auto x=opts.get<Stringkey("configdump")>();
			if(x!=nullptr)
				{return configDump(maike,*x);}
			}

		/*	{
			auto x=opts.get<Stringkey("targets")>();
			if(x==nullptr)
				{maike.targetsBuild();}
			}*/
		}
	catch(const ErrorMessage& message)
		{
		fprintf(stderr,"%s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
