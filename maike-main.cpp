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
#include "maike_base.hpp"
#include "writebuffer.hpp"
#include "graphedgewriterdot.hpp"
#include "variant.hpp"
#include <cstdio>
#include <memory>
#include <cstring>

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
	versionPrint(dest);
	return 0;
	}

static int about(const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	about(dest);
	return 0;
	}


static int loadPathPrint(const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	loadPath(dest);
	return 0;
	}

static void configfilesLoadClean(Session& maike,const std::vector<std::string>* files)
	{
	configAppendDefault(maike);
	if(files==nullptr)
		{
		try
			{
			FileIn source("maikeconfig.json");
			configAppend(maike,source);
			}
		catch(const ErrorMessage& msg)
			{fprintf(stderr,"(!) %s. Loading default configuration.\n\n",msg.messageGet());}
		return;
		}
	auto ptr=files->data();
	auto ptr_end=ptr+files->size();
	while(ptr!=ptr_end)
		{
		FileIn source(ptr->c_str());
		configAppend(maike,source);
		++ptr;
		}
	}

static void configfilesLoad(Session& maike,const std::vector<std::string>* files)
	{
	if(files==nullptr)
		{
		configAppendDefault(maike);
		configAppendCommon(maike);
		return;
		}
	auto ptr=files->data();
	auto ptr_end=ptr+files->size();
	while(ptr!=ptr_end)
		{
		FileIn source(ptr->c_str());
		configAppend(maike,source);
		++ptr;
		}
	}

static std::vector<std::string> hookDataExtract(const char* str)
	{
	std::vector<std::string> ret;
	std::string buffer;
	while(1)
		{
		auto ch_in=*str;
		switch(ch_in)
			{
			case ',':
				if(ret.size()==0)
					{exceptionRaise(ErrorMessage("Missing hook filename",{}));}
				ret.push_back(buffer);
				buffer.clear();
				break;
			case ':':
				ret.push_back(buffer);
				buffer.clear();
				break;
			case '\0':
				ret.push_back(buffer);
				return ret;
			default:
				buffer+=ch_in;
			}
		++str;
		}
	}

static std::vector<const char*> stringsExtract(std::vector<std::string>&& strings)=delete;

static std::vector<const char*> stringsExtract(const std::vector<std::string>& strings)
	{
	std::vector<const char*> ret;
	auto ptr=strings.data();
	auto ptr_end=ptr+strings.size();
	while(ptr!=ptr_end)
		{
		ret.push_back(ptr->c_str());
		++ptr;
		}
	return std::move(ret);
	}

static void hooksLoad(Session& maike,const std::vector<std::string>& hook_strings)
	{
	auto ptr=hook_strings.data();
	auto ptr_end=ptr + hook_strings.size();
	while(ptr!=ptr_end)
		{
		auto hd=hookDataExtract(ptr->c_str());
		auto strings=stringsExtract(hd);
		hookRegister(maike,strings[0],{strings.data() + 1,strings.data() + hd.size()});
		++ptr;
		}
	}

static Twins<std::string> hookConfigExtract(const char* hook_str)
	{
	Twins<std::string> ret;
	int state=0;
	while(1)
		{
		auto ch_in=*hook_str;
		switch(state)
			{
			case 0:
				switch(ch_in)
					{
					case '\0':
						return std::move(ret);
					case ':':
						++state;
						break;
					default:
						ret.first+=ch_in;
					}
				break;
			case 1:
				ret.second+='{';
				ret.second+=hook_str;
				ret.second+='}';
				return std::move(ret);
			}
		++hook_str;
		}
	}

namespace
	{
	class StringReader:public DataSource
		{
		public:
			StringReader(const char* str,size_t n):r_str(str),m_n(n)
				{}

			size_t read(void* buffer,size_t n)
				{
				auto n_read=std::min(n,m_n);
				memcpy(buffer,r_str,n_read);
				m_n-=n_read;
				return n_read;
				}

			const char* nameGet() const noexcept
				{return "Command line buffer";}

			void destroy(){}

		private:
			const char* r_str;
			size_t m_n;
		};
	}

static void hooksConfig(Session& maike,const std::vector<std::string>& config_strings)
	{
	auto ptr=config_strings.data();
	auto ptr_end=ptr + config_strings.size();
	while(ptr!=ptr_end)
		{
		auto content=hookConfigExtract(ptr->c_str());
		StringReader reader(content.second.c_str(),content.second.size());
		auto rc=resourceObjectCreate(reader);
		hookConfigAppend(maike,content.first.c_str(),rc);
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
	targetsListAll(maike,dest);
	return 0;
	}

static int targetsListLeaf(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	targetsListLeaf(maike,dest);
	return 0;
	}

static int targetsListExternal(const Maike::Session& maike,const std::vector<std::string>& filename)
	{
	auto dest=fileGet(filename);
	targetsListExternal(maike,dest);
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

static int clean(Maike::Session& maike,const std::vector<std::string>* targets)
	{
	if(targets==nullptr)
		{
		clean(maike);
		return 0;
		}

	auto ptr=targets->data();
	auto ptr_end=ptr+targets->size();
	while(ptr!=ptr_end)
		{
		clean(maike,ptr->c_str());
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

	auto id_range=targetIdRangeGet(maike);
	std::vector<uint8_t> visited(id_range.second-id_range.first,0);
	auto ptr=targets->data();
	auto ptr_end=ptr+targets->size();
	while(ptr!=ptr_end)
		{
		graphDump(maike,graphwriter,ptr->c_str(),visited.data(),id_range.first);
		++ptr;
		}
	return 0;
	}

static int graphInvDumpDOT(Maike::Session& maike
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

	auto id_range=targetIdRangeGet(maike);
	std::vector<uint8_t> visited(id_range.second-id_range.first,0);
	auto ptr=targets->data();
	auto ptr_end=ptr+targets->size();
	while(ptr!=ptr_end)
		{
		graphInvDump(maike,graphwriter,ptr->c_str(),visited.data(),id_range.first);
		++ptr;
		}
	return 0;
	}

static int databaseDumpJSON(Maike::Session& maike
	,const std::vector<std::string>* targets
	,const std::vector<std::string>& filename)
	{
	if(targets==nullptr)
		{targetsDump(maike).write(fileGet(filename));}
	else
		{
		auto ptr=targets->data();
		auto ptr_end=ptr+targets->size();
		auto db=resourceObjectCreate(ResourceObject::Type::ARRAY);
		while(ptr!=ptr_end)
			{
			targetDump(maike,db,ptr->c_str());
			++ptr;
			}
		db.write(fileGet(filename));
		}
	return 0;
	}

static int targetsDumpTSV(Maike::Session& maike
	,const std::vector<std::string>* targets
	,const std::vector<std::string>& filename)
	{
	auto file=fileGet(filename);
	targetsDumpTSVHeader(file);
	if(targets==nullptr)
		{targetsDumpTSV(maike,file);}
	else
		{
		auto ptr=targets->data();
		auto ptr_end=ptr+targets->size();
		while(ptr!=ptr_end)
			{
			targetDumpTSV(maike,file,ptr->c_str());
			++ptr;
			}
		}
	return 0;
	}

int main(int argc,char** argv)
	{
	try
		{
		auto maike_ptr=sessionCreate();
		auto& maike=*maike_ptr.get();

		Options opts{Twins<const char* const*>(argv,argv+argc)};

		auto x=opts.get<Stringkey("help")>();
		if(x!=nullptr)
			{return helpPrint(opts,*x);}

		x=opts.get<Stringkey("version")>();
		if(x!=nullptr)
			{return versionPrint(*x);}


		x=opts.get<Stringkey("about")>();
		if(x!=nullptr)
			{return about(*x);}

		x=opts.get<Stringkey("load-path")>();
		if(x!=nullptr)
			{return loadPathPrint(*x);}

		if(opts.get<Stringkey("no-sysvars")>()==nullptr)
			{sysvarsLoad(maike);}

		if(opts.get<Stringkey("configclean")>()==nullptr)
			{configfilesLoad(maike,opts.get<Stringkey("configfiles")>());}
		else
			{configfilesLoadClean(maike,opts.get<Stringkey("configfiles")>());}

		x=opts.get<Stringkey("hooks-load")>();
		if(x!=nullptr)
			{hooksLoad(maike,*x);}

		x=opts.get<Stringkey("hooks-config")>();
		if(x!=nullptr)
			{hooksConfig(maike,*x);}

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

		x=opts.get<Stringkey("dump-graph-inv-dot")>();
		if(x!=nullptr)
			{return graphInvDumpDOT(maike,opts.get<Stringkey("targets")>(),*x);}

		x=opts.get<Stringkey("dump-database-json")>();
		if(x!=nullptr)
			{return databaseDumpJSON(maike,opts.get<Stringkey("targets")>(),*x);}

		x=opts.get<Stringkey("remove-orphans")>();
		if(x!=nullptr)
			{removeOrphans(maike);}

		x=opts.get<Stringkey("clean")>();
		if(x!=nullptr)
			{return clean(maike,opts.get<Stringkey("targets")>());}

		if(x!=nullptr)
			{return 0;}

		targetsCompile(maike,opts.get<Stringkey("targets")>());

		x=opts.get<Stringkey("dump-targets-tsv")>();
		if(x!=nullptr)
			{return targetsDumpTSV(maike,opts.get<Stringkey("targets")>(),*x);}
		}
	catch(const ErrorMessage& message)
		{
		fprintf(stderr,"%s\n",message.messageGet());
		return -1;
		}
	return 0;
	}
