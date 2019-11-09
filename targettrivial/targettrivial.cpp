//@	{"targets":[{"name":"targettrivial.o","type":"object"}]}

#include "targettrivial.hpp"
#include "../resourceobject.hpp"
#include "../fileutils.hpp"
#include "../writebuffer.hpp"
#include "../parametersetmapfixed.hpp"
#include "../readbuffer.hpp"
#include "../stdstream.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../exceptionhandler.hpp"


using namespace Maike;

TargetTrivial::TargetTrivial(const Command& cmd, const char* name,const char* name_src
				,const char* in_dir,const char* root,size_t id):
				TargetBase(name,name_src,in_dir,root,id), r_cmd{cmd}
	{
	}

void TargetTrivial::dumpDetails(ResourceObject&) const
	{
	}

bool TargetTrivial::upToDate(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir) const
	{
	auto name_out=dircat(target_dir,nameGet());

	if(FileUtils::newer(sourceNameGet(),name_out.c_str()))
		{return 0;}

	auto up_to_date=[&name_out](const char* name,Dependency::Relation)
		{return !FileUtils::newer(name,name_out.c_str());};

	if(!dependenciesProcess(target_dir,dependencies(),USE_TOOL|USE_MISC,up_to_date))
		{return 0;}

	return 1;
	}

static void execute(const Command& cmd, const char* src, const char* dest, const char* current_dir)
	{
	ParameterSetMapFixed<Stringkey("source"),Stringkey("target"),Stringkey("in_dir")> params;
	params.get<Stringkey("source")>().push_back(std::string(src));
	params.get<Stringkey("target")>().push_back(std::string(dest));
	params.get<Stringkey("in_dir")>().push_back(std::string(current_dir));

	const ParameterSet* paramset[]={&params};
	auto compiler=cmd.execute(Pipe::REDIRECT_STDERR
		,{paramset,paramset + 1 });
	auto stream=compiler.stderrCapture();
	ReadBuffer rb(*stream.get());
	WriteBuffer wb(StdStream::error());
	while(!rb.eof())
		{wb.write(rb.byteRead());}
	auto res=compiler.exitStatusGet();
	if(processFailed(res))
		{
		if(res.killedWas())
			{
			exceptionRaise(ErrorMessage("#0;: It was not possible to generate #1;. "
				"The compiler was killed with signal #2;",{src,dest,res.errorCodeGet()}));
			}
		else
			{
			exceptionRaise(ErrorMessage("#0;: It was not possible to generate #1;. "
				"The compiler returned status code #2;",{src,dest,res.errorCodeGet()}));
			}
		}
	}

void TargetTrivial::compileImpl(Twins<const Dependency*>
	,Twins<const Dependency*>
	,const char* target_dir)
	{
	auto name_out=dircat(target_dir,nameGet());
	execute(r_cmd, sourceNameGet(), name_out.c_str(), inDirGet());
	}

TargetTrivial::~TargetTrivial() noexcept
	{
	}


TargetTrivial* TargetTrivial::create(const Command& cmd, const char* name,const char* name_src
				,const char* in_dir,const char* root,size_t id)
	{return new TargetTrivial(cmd,name,name_src,in_dir,root,id);}

void TargetTrivial::destroy() noexcept
	{delete this;}
