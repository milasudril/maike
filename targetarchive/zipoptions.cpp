//@	{
//@	 "targets":[{"name":"zipoptions.o","type":"object"}]
//@	}

#include "zipoptions.hpp"
#include "../resourceobject.hpp"

using namespace Maike;

Zipoptions::Zipoptions(const ResourceObject& options)
	{
	configAppend(options);
	}

Zipoptions& Zipoptions::configAppend(const ResourceObject& options)
	{
	if(options.objectExists("create"))
		{m_create=Command(options.objectGet("create"));}

	if(options.objectExists("rename"))
		{m_create=Command(options.objectGet("rename"));}

	return *this;
	}

Zipoptions& Zipoptions::configAppendDefault()
	{
	m_create.nameSet("zip")
		.argumentAppend("-@")
		.argumentAppend("{target}");

	m_rename.nameSet("zipnote")
		.argumentAppend("-w")
		.argumentAppend("{target}");

	return *this;
	}

void Zipoptions::configDump(ResourceObject& options) const
	{
		{
		auto create=options.createObject();
		m_create.configDump(create);
		create.objectSet("command",std::move(create));
		}

		{
		auto rename=options.createObject();
		m_rename.configDump(rename);
		options.objectSet("rename",std::move(rename));
		}
	}

