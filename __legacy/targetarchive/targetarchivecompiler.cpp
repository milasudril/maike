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
#include "../fileutils.hpp"
#include <cstring>

using namespace Maike;

TargetArchiveCompiler::TargetArchiveCompiler(const ParameterSetDumpable& sysvars):
   r_sysvars(sysvars)
{
	configClear();
}


namespace
{
	class ReadCallback
	{
	public:
		explicit ReadCallback(DataSource* src): r_src(src)
		{
		}

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
			{
				wb.write("Error: ").write(message.messageGet());
			}
		}

	private:
		DataSource* r_src;
	};
}

static void dataProcess(Pipe& compiler, Twins<const char* const*> files)
{
	auto standard_error = compiler.stderrCapture();
	auto standard_output = compiler.stdoutCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
	auto standard_input = compiler.stdinCapture();
	WriteBuffer wb(*standard_input.get());
	while(files.first != files.second)
	{
		wb.write(*files.first).write("\n");
		++files.first;
	}
}

static void
dataProcess(Pipe& compiler, Twins<const char* const*> files, const char* in_dir, const char* root)
{
	auto standard_error = compiler.stderrCapture();
	auto standard_output = compiler.stdoutCapture();
	Thread<ReadCallback> stderr_reader(ReadCallback{standard_error.get()});
	auto standard_input = compiler.stdinCapture();
	WriteBuffer wb(*standard_input.get());
	while(files.first != files.second)
	{
		if(strcmp(*files.first, "."))
		{
			wb.write("@ ").write(*files.first).write("\n");
			auto name_new = dircat(root, rootStrip(*files.first, in_dir));
			wb.write("@=").write(name_new.c_str()).write("\n").write("@ ()");
			++files.first;
			if(files.first != files.second) { wb.write("\n"); }
		}
		else
		{
			++files.first;
		}
	}
}

static std::string placeholderSubstitute(const char* string_template, const char* substitute)
{
	std::string ret;
	bool escape = 0;
	while(true)
	{
		auto ch_in = *string_template;
		if(escape)
		{
			escape = 0;
			ret += ch_in;
		}
		else
		{
			switch(ch_in)
			{
				case '\\': escape = 1; break;
				case '^': ret += substitute; break;
				case '\0': return std::move(ret);
				default: ret += ch_in;
			}
		}
		++string_template;
	}
}

int TargetArchiveCompiler::tar(Twins<const char* const*> files,
                               const char* name,
                               const char* target_dir,
                               const char* root,
                               const char* compressor) const
{
	ParameterSetMapFixed<Stringkey("target_strip"),
	                     Stringkey("root_append"),
	                     Stringkey("compressor"),
	                     Stringkey("target")>
	   params;

	{
		auto compressor_val = m_tar.compressorGet(compressor);
		if(compressor_val == nullptr)
		{
			exceptionRaise(
			   ErrorMessage("#0;: It was not possible create the archive. Unregistred compressor #1;.",
			                {name, compressor}));
		}
		params.get<Stringkey("compressor")>().push_back(std::string(compressor_val));
	}

	auto target_dir_temp = *target_dir ? std::string(target_dir) + '/' : std::string(target_dir);
	auto root_temp = *root ? std::string(root) + '/' : std::string(root);

	params.get<Stringkey("target")>().push_back(dircat(target_dir, name));
	params.get<Stringkey("target_strip")>().push_back(
	   placeholderSubstitute(m_tar.targetStripGet(), target_dir_temp.c_str()));
	params.get<Stringkey("root_append")>().push_back(
	   placeholderSubstitute(m_tar.rootAppendGet(), root_temp.c_str()));

	const ParameterSet* paramset_tot[] = {&r_sysvars, &params};
	auto pipe =
	   m_tar.commandGet().execute(Pipe::REDIRECT_STDERR | Pipe::REDIRECT_STDIN | Pipe::REDIRECT_STDOUT,
	                              {paramset_tot, paramset_tot + 2});

	dataProcess(pipe, files);

	auto ret = pipe.exitStatusGet();
	if(processFailed(ret))
	{
		FileUtils::remove(params.get<Stringkey("target")>().front().c_str());
		if(ret.killedWas())
		{
			exceptionRaise(
			   ErrorMessage("It was not possible to create #0;. The archiver was killed with signal #1;.",
			                {name, ret.errorCodeGet()}));
		}
		else
		{
			exceptionRaise(
			   ErrorMessage("It was not possible to create #0;. The archiver returned status code #1;.",
			                {name, ret.errorCodeGet()}));
		}
	}
	return 0;
}

int TargetArchiveCompiler::zip(Twins<const char* const*> files,
                               const char* name,
                               const char* target_dir,
                               const char* root) const
{
	ParameterSetMapFixed<Stringkey("target")> params;
	params.get<Stringkey("target")>().push_back(dircat(target_dir, name));

	FileUtils::remove(params.get<Stringkey("target")>().front().c_str());

	const ParameterSet* paramset_tot[] = {&r_sysvars, &params};
	{
		auto pipe =
		   m_zip.createGet().execute(Pipe::REDIRECT_STDERR | Pipe::REDIRECT_STDIN | Pipe::REDIRECT_STDOUT,
		                             {paramset_tot, paramset_tot + 2});

		dataProcess(pipe, files);
		auto ret = pipe.exitStatusGet();
		if(processFailed(ret))
		{
			FileUtils::remove(params.get<Stringkey("target")>().front().c_str());
			if(ret.killedWas())
			{
				exceptionRaise(
				   ErrorMessage("It was not possible to create #0;. The archiver was killed with signal #1;",
				                {name, ret.errorCodeGet()}));
			}
			else
			{
				exceptionRaise(
				   ErrorMessage("It was not possible to create #0;. The archiver returned status code #1;",
				                {name, ret.errorCodeGet()}));
			}
		}
	}
	{
		auto pipe =
		   m_zip.renameGet().execute(Pipe::REDIRECT_STDERR | Pipe::REDIRECT_STDIN | Pipe::REDIRECT_STDOUT,
		                             {paramset_tot, paramset_tot + 2});

		dataProcess(pipe, files, target_dir, root);
		auto ret = pipe.exitStatusGet();
		if(processFailed(ret))
		{
			FileUtils::remove(params.get<Stringkey("target")>().front().c_str());
			if(ret.killedWas())
			{
				exceptionRaise(
				   ErrorMessage("It was not possible to create #0;. The archiver was killed with signal #1;",
				                {name, ret.errorCodeGet()}));
			}
			else
			{
				exceptionRaise(
				   ErrorMessage("It was not possible to create #0;. The archiver returned status code #1;",
				                {name, ret.errorCodeGet()}));
			}
		}
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
	m_zip.configAppendDefault();
	return *this;
}


TargetArchiveCompiler& TargetArchiveCompiler::configAppend(const ResourceObject& archiveoptions)
{
	if(archiveoptions.objectExists("tar")) { m_tar = Taroptions(archiveoptions.objectGet("tar")); }
	if(archiveoptions.objectExists("zip")) { m_zip = Zipoptions(archiveoptions.objectGet("zip")); }
	return *this;
}

void TargetArchiveCompiler::configDump(ResourceObject& archiveoptions) const
{
	{
		auto tar = archiveoptions.createObject();
		m_tar.configDump(tar);
		archiveoptions.objectSet("tar", std::move(tar));
	}

	{
		auto zip = archiveoptions.createObject();
		m_zip.configDump(zip);
		archiveoptions.objectSet("zip", std::move(zip));
	}
}
