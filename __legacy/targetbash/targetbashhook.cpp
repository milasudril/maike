//@	{"targets":[{"name":"targetbashhook.o","type":"object"}]}

#include "targetbashhook.hpp"

using namespace Maike;

TargetBashHook::TargetBashHook(const ParameterSetDumpable& params):
   m_intpret(params), m_loader(m_intpret)
{
}

TargetBashHook* TargetBashHook::create(const ParameterSetDumpable& params)
{
	return new TargetBashHook(params);
}

void TargetBashHook::destroy() noexcept
{
	delete this;
}

void TargetBashHook::configClear()
{
	m_intpret.configClear();
}


TargetBashHook& TargetBashHook::configAppendDefault()
{
	m_intpret.configAppendDefault();
	return *this;
}

TargetBashHook& TargetBashHook::configAppend(const ResourceObject& options)
{
	m_intpret.configAppend(options);
	return *this;
}

void TargetBashHook::configDump(ResourceObject& options) const
{
	m_intpret.configDump(options);
}
