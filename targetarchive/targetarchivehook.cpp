//@	{"targets":[{"name":"targetarchivehook.o","type":"object"}]}

#include "targetarchivehook.hpp"

using namespace Maike;

TargetArchiveHook::TargetArchiveHook(const ParameterSetDumpable& params):
	m_intpret(params),m_factory(m_intpret)
	{
	}

TargetArchiveHook* TargetArchiveHook::create(const ParameterSetDumpable& params)
	{return new TargetArchiveHook(params);}

void TargetArchiveHook::destroy() noexcept
	{delete this;}

void TargetArchiveHook::configClear()
	{
	m_intpret.configClear();
	}


TargetArchiveHook& TargetArchiveHook::configAppendDefault()
	{
	m_intpret.configAppendDefault();
	return *this;
	}

TargetArchiveHook& TargetArchiveHook::configAppend(const ResourceObject& archiveoptions)
	{
	m_intpret.configAppend(archiveoptions);
	return *this;
	}

void TargetArchiveHook::configDump(ResourceObject& archiveoptions) const
	{
	m_intpret.configDump(archiveoptions);
	}
