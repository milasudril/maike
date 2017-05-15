//@	{"targets":[{"name":"targetoctavehook.o","type":"object"}]}

#include "targetoctavehook.hpp"

using namespace Maike;

TargetOctaveHook::TargetOctaveHook(const ParameterSetDumpable& params):
	m_intpret(params),m_factory(m_intpret)
	{
	}

TargetOctaveHook* TargetOctaveHook::create(const ParameterSetDumpable& params)
	{return new TargetOctaveHook(params);}

void TargetOctaveHook::destroy() noexcept
	{delete this;}

void TargetOctaveHook::configClear()
	{
	m_intpret.configClear();
	}


TargetOctaveHook& TargetOctaveHook::configAppendDefault()
	{
	m_intpret.configAppendDefault();
	return *this;
	}

TargetOctaveHook& TargetOctaveHook::configAppend(const ResourceObject& octaveoptions)
	{
	m_intpret.configAppend(octaveoptions);
	return *this;
	}

void TargetOctaveHook::configDump(ResourceObject& octaveoptions) const
	{
	m_intpret.configDump(octaveoptions);
	}
