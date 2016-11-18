//@	{"targets":[{"name":"targetarchivecompiler.o","type":"object"}]}

#include "targetarchivecompiler.hpp"
#include "../resourceobject.hpp"
#include "../parametersetmapfixed.hpp"
#include "../readbuffer.hpp"
#include "../errormessage.hpp"
#include "../variant.hpp"
#include "../exceptionhandler.hpp"
#include "../thread.hpp"
#include "../parametersetdumpable.hpp"
#include "../stdstream.hpp"
#include "../writebuffer.hpp"
#include "../pathutils.hpp"

using namespace Maike;

TargetArchiveCompiler::TargetArchiveCompiler(const ParameterSetDumpable& sysvars)
	:r_sysvars(sysvars)
	{
	configClear();
	}


namespace
	{
	class ReadCallback
		{
		public:
			explicit ReadCallback(DataSource* src):r_src(src)
				{}

			void operator()()
				{
				WriteBuffer wb(StdStream::error());
				try
					{
					ReadBuffer rb(*r_src);
					while(!rb.eof())
						{
						wb.write(rb.byteRead());
						}
					}
				catch(const ErrorMessage& message)
					{wb.write("Error: ").write(message.messageGet());}
				}

		private:
			DataSource* r_src;
		};
	}

static void dataProcess(Pipe& compiler,Twins<const char* const*> files)
	{
	auto standard_error=compiler.stderrCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
	auto standard_input=compiler.stdinCapture();
	WriteBuffer wb(*standard_input.get());
	while(files.first!=files.second)
		{
		wb.write(*files.first).write(static_cast<uint8_t>('\n'));
		++files.first;
		}
//	sysvars.configDump().write(*standard_input.get());
	}

static std::string placeholderSubstitute(const char* string_template
	,const char* substitute)
	{
	std::string ret;
	bool escape=0;
	while(true)
		{
		auto ch_in=*string_template;
		if(escape)
			{
			escape=0;
			ret+=ch_in;
			}
		else
			{
			switch(ch_in)
				{
				case '\\':
					escape=1;
					break;
				case '^':
					ret+=substitute;
					break;
				case '\0':
					return std::move(ret);
				default:
					ret+=ch_in;
				}
			}
		++string_template;
		}
	}

int TargetArchiveCompiler::tar(Twins<const char* const*> files,const char* name
	,const char* target_dir,const char* root
	,const char* compressor) const
	{
	ParameterSetMapFixed<
		 Stringkey("target_strip")
		,Stringkey("root_append")
		,Stringkey("compressor")
		,Stringkey("target")
		> params;

		{
		auto compressor_val=m_tar.compressorGet(compressor);
		if(compressor_val==nullptr)
			{
			exceptionRaise(ErrorMessage("#0;: It was not possible create the archive. Unregistred compressor #1;."
				,{name,compressor}));
			}
		params.get<Stringkey("compressor")>().push_back(std::string(compressor_val));
		}

	auto target_dir_temp=*target_dir?std::string(target_dir)+'/'
		:std::string(target_dir);
	auto root_temp=*root?std::string(root)+'/'
		:std::string(root);

	params.get<Stringkey("target")>().push_back(dircat(target_dir,name));
	params.get<Stringkey("target_strip")>()
		.push_back(placeholderSubstitute(m_tar.targetStripGet(),target_dir_temp.c_str()));
	params.get<Stringkey("root_append")>()
		.push_back(placeholderSubstitute(m_tar.rootAppendGet(),root_temp.c_str()));

	const ParameterSet* paramset_tot[]={&r_sysvars,&params};
	auto pipe=m_tar.commandGet().execute(Pipe::REDIRECT_STDERR|Pipe::REDIRECT_STDIN
		,{paramset_tot,paramset_tot + 2});

	dataProcess(pipe,files);

	auto ret=pipe.exitStatusGet();
	if(ret!=0)
		{
		exceptionRaise(ErrorMessage("It was not possible to create #0;. The archiver returned status code #0;"
			,{name,ret}));
		}
	return 0;
	}

void TargetArchiveCompiler::configClear()
	{
	m_tar.configClear();
	}

TargetArchiveCompiler& TargetArchiveCompiler::configAppendDefault()
	{
	m_tar.configAppendDefault();
	return *this;
	}


TargetArchiveCompiler& TargetArchiveCompiler::configAppend(const ResourceObject& archiveoptions)
	{
	if(archiveoptions.objectExists("tar"))
		{m_tar=Taroptions(archiveoptions.objectGet("tar"));}
	return *this;
	}

void TargetArchiveCompiler::configDump(ResourceObject& archiveoptions) const
	{
	auto tar=archiveoptions.createObject();
	m_tar.configDump(tar);
	archiveoptions.objectSet("tar",std::move(tar));
	}
