//@	{
//@	 "targets":[{"name":"targettrivialoptions.o","type":"object"}]
//@	}

#include "targettrivialoptions.hpp"
#include "../resourceobject.hpp"

Maike::TargetTrivialOptions::TargetTrivialOptions()
	{
	}

Maike::TargetTrivialOptions::TargetTrivialOptions(const ResourceObject& options)
	{
	configClear();
	configAppend(options);
	}

void Maike::TargetTrivialOptions::configClear()
	{
	m_build_cmd = Command{};
	}

Maike::TargetTrivialOptions& Maike::TargetTrivialOptions::configAppendDefault()
	{
	configClear();
	return *this;
	}

Maike::TargetTrivialOptions& Maike::TargetTrivialOptions::configAppend(const ResourceObject& opts)
	{
	if(opts.objectExists("command"))
		{m_build_cmd=Command(opts.objectGet("command"));}
	if(opts.objectExists("filename_ext"))
		{m_filename_ext=static_cast<const char*>(opts.objectGet("filename_ext"));}
	return *this;
	}

Maike::TargetTrivialOptions& Maike::TargetTrivialOptions::configAppend(const TargetTrivialOptions& opts)
	{
	m_build_cmd=opts.m_build_cmd;
	return *this;
	}

void Maike::TargetTrivialOptions::configDump(ResourceObject& options) const
	{
		{
		auto cmd=options.createObject();
		m_build_cmd.configDump(cmd);
		options.objectSet("command",std::move(cmd));
		}

		{
		auto filename_ext=options.create(m_filename_ext.c_str());
		options.objectSet("filename_ext",std::move(filename_ext));
		}
	}
