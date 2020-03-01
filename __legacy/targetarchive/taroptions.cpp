//@	{
//@	 "targets":[{"name":"taroptions.o","type":"object"}]
//@	}

#include "taroptions.hpp"
#include "../resourceobject.hpp"

using namespace Maike;

Taroptions::Taroptions(const ResourceObject& options)
{
	configAppend(options);
}

Taroptions& Taroptions::configAppend(const ResourceObject& options)
{
	if(options.objectExists("command")) { m_command = Command(options.objectGet("command")); }

	if(options.objectExists("compressors"))
	{
		auto compressors = options.objectGet("compressors");
		auto i = compressors.objectIteratorGet();
		while(!i.endAt())
		{
			auto p = i.get();
			auto key = Stringkey(p.first);
			m_compressors[key] = {std::string(p.first), std::string(static_cast<const char*>(p.second))};
			i.next();
		}
	}

	if(options.objectExists("target_strip"))
	{ m_target_strip = std::string(static_cast<const char*>(options.objectGet("target_strip"))); }

	if(options.objectExists("root_append"))
	{ m_root_append = std::string(static_cast<const char*>(options.objectGet("root_append"))); }

	return *this;
}

Taroptions& Taroptions::configAppendDefault()
{
	m_command.nameSet("tar")
	   .argumentAppend("{target_strip}")
	   .argumentAppend("{root_append}")
	   .argumentAppend("{compressor}")
	   .argumentAppend("-cf")
	   .argumentAppend("{target}")
	   .argumentAppend("-T")
	   .argumentAppend("-");

	m_compressors[Stringkey("gzip")] = {"gzip", "-z"};
	m_compressors[Stringkey("bzip2")] = {"bzip2", "-j"};
	m_compressors[Stringkey("lzma")] = {"lzma", "-J"};
	m_compressors[Stringkey("gzip")] = {"gzip", "-z"};

	m_target_strip = std::string("--transform=s,\\^^,,g");
	m_root_append = std::string("--transform=s,\\\\(.*\\\\),^\\\\1,g");
	return *this;
}

void Taroptions::configDump(ResourceObject& options) const
{
	options.objectSet("target_strip", options.create(m_target_strip.c_str()))
	   .objectSet("root_append", options.create(m_root_append.c_str()));
	{
		auto command = options.createObject();
		m_command.configDump(command);
		options.objectSet("command", std::move(command));
	}

	{
		auto compressors = options.createObject();
		auto i = m_compressors.begin();
		while(i != m_compressors.end())
		{
			compressors.objectSet(i->second.first.c_str(), options.create(i->second.second.c_str()));
			++i;
		}
		options.objectSet("compressors", std::move(compressors));
	}
}

const char* Taroptions::compressorGet(const char* compressor) const noexcept
{
	auto i = m_compressors.find(Stringkey(compressor));
	if(i == m_compressors.end()) { return nullptr; }
	return i->second.second.c_str();
}
