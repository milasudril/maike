//@	[
//@		["windows"
//@			,{"targets":[{"name":"maike.exe","type":"application"}]}]
//@		,["1",{"targets":[{"name":"maike","type":"application"}]}]
//@	]

#include "errormessage.hpp"
#include "options.hpp"
#include "fileout.hpp"
#include "filein.hpp"
#include "resourceobject.hpp"
#include "session.hpp"
#include "maike.hpp"
#include "writebuffer.hpp"
#include "graphedgewriterdot.hpp"
#include <cstdio>

using namespace Maike;

FileOut fileGet(const std::vector<std::string>& filename)
	{
	if(filename.size()==0)
		{return FileOut();}
	return FileOut(filename.begin()->c_str());
	}

static int helpPrint(const Maike::Options& opts,const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	opts.printHelp(dest);
	return 0;
	}

static int versionPrint(const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	WriteBuffer wb(dest);
	versionPrint(wb);
	return 0;
	}

static void configfilesLoad(Session& maike,const std::vector<std::string>* files)
	{
	if(files==nullptr)
		{
		try
			{
			FileIn source("maikeconfig.json");
			configLoad(maike,source);
			}
		catch(...){}
		return;
		}
	auto ptr=files->data();
	auto ptr_end=ptr+files->size();
	while(ptr!=ptr_end)
		{
		FileIn source(ptr->c_str());
		configLoad(maike,source);
		++ptr;
		}
	}

static int configDump(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	configDump(maike,dest);
	return 0;
	}

static int targetsListAll(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	WriteBuffer wb(dest);
	targetsListAll(maike,wb);
	return 0;
	}

static int targetsListLeaf(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	WriteBuffer wb(dest);
	targetsListLeaf(maike,wb);
	return 0;
	}

static int targetsListExternal(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	WriteBuffer wb(dest);
	targetsListExternal(maike,wb);
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

static int graphDumpDOT(Maike::Session& maike
	,const std::vector<std::string>* targets
	,const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	WriteBuffer wb(dest);
	GraphEdgeWriterDOT graphwriter(wb);
	if(targets==nullptr)
		{
		graphDump(maike,graphwriter);
		return 0;
		}

	auto id_range=maike.targetIdRangeGet();
	std::vector<uint8_t> visited((id_range.second-id_range.first) + 1,0);
	auto ptr=targets->data();
	auto ptr_end=ptr+targets->size();
	while(ptr!=ptr_end)
		{
		graphDump(maike,graphwriter,ptr->c_str(),visited.data(),id_range.first);
		++ptr;
		}
	return 0;
	}

static int targetsDump(Maike::Session& maike
	,const std::vector<std::string>* targets
	,const std::vector<std::string>& filename)
	{
	ResourceObject db(ResourceObject::Type::ARRAY);
	if(targets==nullptr)
		{targetsDump(maike,db);}
	else
		{
		auto ptr=targets->data();
		auto ptr_end=ptr+targets->size();
		while(ptr!=ptr_end)
			{
			targetDump(maike,db,ptr->c_str());
			++ptr;
			}
		}

	auto file=fileGet(filename);
	db.write(file);

	return 0;
	}

int main(int argc,char** argv)
	{
	try
		{
		Session maike;
		Options opts{Twins<const char* const*>(argv,argv+argc)};

		auto x=opts.get<Stringkey("help")>();
		if(x!=nullptr)
			{return helpPrint(opts,*x);}

		x=opts.get<Stringkey("version")>();
		if(x!=nullptr)
			{return versionPrint(*x);}

		if(opts.get<Stringkey("no-sysvars")>()!=nullptr)
			{maike.configClear();}

		configfilesLoad(maike,opts.get<Stringkey("configfiles")>());

		x=opts.get<Stringkey("configdump")>();
		if(x!=nullptr)
			{return configDump(maike,*x);}

		x=opts.get<Stringkey("list-all-targets")>();
		if(x!=nullptr)
			{return targetsListAll(maike,*x);}

		x=opts.get<Stringkey("list-leaf-targets")>();
		if(x!=nullptr)
			{return targetsListLeaf(maike,*x);}

		x=opts.get<Stringkey("list-leaf-targets")>();
		if(x!=nullptr)
			{return targetsListLeaf(maike,*x);}

		x=opts.get<Stringkey("list-external-targets")>();
		if(x!=nullptr)
			{return targetsListExternal(maike,*x);}

		x=opts.get<Stringkey("dump-graph-dot")>();
		if(x!=nullptr)
			{return graphDumpDOT(maike,opts.get<Stringkey("targets")>(),*x);}

		x=opts.get<Stringkey("dump-database")>();
		if(x!=nullptr)
			{return targetsDump(maike,opts.get<Stringkey("targets")>(),*x);}

		targetsCompile(maike,opts.get<Stringkey("targets")>());
		}
	catch(const ErrorMessage& message)
		{
		fprintf(stderr,"%s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
