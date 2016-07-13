//@	[
//@		["windows"
//@			,{"targets":[{"name":"maike.exe","type":"application"}]}]
//@		,["1",{"targets":[{"name":"maike","type":"application"}]}]
//@	]

#include "errormessage.hpp"
#include "options.hpp"
#include "datasinkstd.hpp"
#include "fileout.hpp"
#include "resourceobject.hpp"
#include "session.hpp"
#include "maike.hpp"
#include <cstdio>

using namespace Maike;

static int helpPrint(const Maike::Options& opts,const std::vector<std::string>& filename)
	{
	if(filename.size()==0)
		{
		opts.printHelp(DataSinkStd::standard_output);
		return 0;
		}
	opts.printHelp(FileOut{filename.begin()->c_str()});
	return 0;
	}

static int versionPrint(const std::vector<std::string>& filename)
	{
	if(filename.size()==0)
		{
		versionPrint(DataSinkStd::standard_output);
		return 0;
		}
	versionPrint(filename.begin()->c_str());
	return 0;
	}

static void configfilesLoad(Session& maike,const std::vector<std::string>* files)
	{
	if(files==nullptr)
		{
		try
			{configLoad(maike,"maikeconfig.json");}
		catch(...){}
		return;
		}
	auto ptr=files->data();
	auto ptr_end=ptr+files->size();
	while(ptr!=ptr_end)
		{
		configLoad(maike,ptr->c_str());
		++ptr;
		}
	}

static int configDump(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	if(filename.size()==0)
		{
		configDump(maike,DataSinkStd::standard_output);
		return 0;
		}
	configDump(maike,filename.begin()->c_str());
	return 0;
	}

static int targetsListAll(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	if(filename.size()==0)
		{
		targetsListAll(maike,DataSinkStd::standard_output);
		return 0;
		}
	targetsListAll(maike,filename.begin()->c_str());
	return 0;
	}

static int targetsListLeaf(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	if(filename.size()==0)
		{
		targetsListLeaf(maike,DataSinkStd::standard_output);
		return 0;
		}
	targetsListLeaf(maike,filename.begin()->c_str());
	return 0;
	}

static int targetsListExternal(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	if(filename.size()==0)
		{
		targetsListExternal(maike,DataSinkStd::standard_output);
		return 0;
		}
	targetsListExternal(maike,filename.begin()->c_str());
	return 0;
	}

static int targetsCompile(Maike::Session& maike,const std::vector<std::string>* targets)
	{
	if(targets==nullptr)
		{
		targetsCompile(maike);
		return 0;
		}

	auto ptr=targets->data();
	auto ptr_end=ptr+targets->size();
	while(ptr!=ptr_end)
		{
		targetCompile(maike,ptr->c_str());
		++ptr;
		}
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

		configfilesLoad(maike,opts.get<Stringkey("configfiles")>());

			{
			auto x=opts.get<Stringkey("configdump")>();
			if(x!=nullptr)
				{return configDump(maike,*x);}
			}

			{
			auto x=opts.get<Stringkey("list-all-targets")>();
			if(x!=nullptr)
				{return targetsListAll(maike,*x);}
			}

			{
			auto x=opts.get<Stringkey("list-leaf-targets")>();
			if(x!=nullptr)
				{return targetsListLeaf(maike,*x);}
			}

			{
			auto x=opts.get<Stringkey("list-leaf-targets")>();
			if(x!=nullptr)
				{return targetsListLeaf(maike,*x);}
			}

			{
			auto x=opts.get<Stringkey("list-external-targets")>();
			if(x!=nullptr)
				{return targetsListExternal(maike,*x);}
			}

		targetsCompile(maike,opts.get<Stringkey("targets")>());
		}
	catch(const ErrorMessage& message)
		{
		fprintf(stderr,"%s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
