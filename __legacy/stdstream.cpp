//@	{"targets":[{"name":"stdstream.o","type":"object"}]}

#include "stdstream.hpp"
#include "fileout.hpp"

using namespace Maike;

static FileOut s_stderr_default(FileOut::StdStream::ERROR);
static FileOut s_stdout_default(FileOut::StdStream::OUTPUT);

static DataSink* s_stderr = &s_stderr_default;
static DataSink* s_stdout = &s_stdout_default;

DataSink& StdStream::error() noexcept
{
	return *s_stderr;
}

DataSink& StdStream::output() noexcept
{
	return *s_stdout;
}

void StdStream::errorSet(DataSink& sink) noexcept
{
	s_stderr = &sink;
}

void StdStream::outputSet(DataSink& sink) noexcept
{
	s_stdout = &sink;
}
