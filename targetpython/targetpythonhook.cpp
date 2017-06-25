//@	{"targets":[{"name":"targetpythonhook.o","type":"object"}]}

#include "targetpythonhook.hpp"

using namespace Maike;

TargetPythonHook::TargetPythonHook(const ParameterSetDumpable& params):
	m_intpret(params),m_loader(m_intpret)
	{
	}

TargetPythonHook* TargetPythonHook::create(const ParameterSetDumpable& params)
	{return new TargetPythonHook(params);}

void TargetPythonHook::destroy() noexcept
	{delete this;}

void TargetPythonHook::configClear()
	{
	m_intpret.configClear();
	}


TargetPythonHook& TargetPythonHook::configAppendDefault()
	{
	m_intpret.configAppendDefault();
	return *this;
	}

TargetPythonHook& TargetPythonHook::configAppend(const ResourceObject& pythonoptions)
	{
	m_intpret.configAppend(pythonoptions);
	return *this;
	}

void TargetPythonHook::configDump(ResourceObject& pythonoptions) const
	{
	m_intpret.configDump(pythonoptions);
	}
