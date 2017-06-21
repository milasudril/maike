//@	{"targets":[{"name":"targetxmlhook.o","type":"object"}]}

#include "targetxmlhook.hpp"

using namespace Maike;

TargetXMLHook::TargetXMLHook(const ParameterSetDumpable& params):
	m_loader(params)
	{
	}

TargetXMLHook* TargetXMLHook::create(const ParameterSetDumpable& params)
	{return new TargetXMLHook(params);}

void TargetXMLHook::destroy() noexcept
	{delete this;}

void TargetXMLHook::configClear()
	{
	m_loader.configClear();
	}


TargetXMLHook& TargetXMLHook::configAppendDefault()
	{
	m_loader.configAppendDefault();
	return *this;
	}

TargetXMLHook& TargetXMLHook::configAppend(const ResourceObject& xmloptions)
	{
	m_loader.configAppend(xmloptions);
	return *this;
	}

void TargetXMLHook::configDump(ResourceObject& xmloptions) const
	{
	m_loader.configDump(xmloptions);
	}
