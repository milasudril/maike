//@	{"targets":[{"name":"targettrivialhook.o","type":"object"}]}

#include "targettrivialhook.hpp"

using namespace Maike;

Maike::TargetTrivialHook::TargetTrivialHook(): m_loader{m_opts}
{
}

TargetTrivialHook* TargetTrivialHook::create()
{
	return new TargetTrivialHook();
}

void TargetTrivialHook::destroy() noexcept
{
	delete this;
}

void TargetTrivialHook::configClear()
{
	m_opts.configClear();
}


TargetTrivialHook& TargetTrivialHook::configAppendDefault()
{
	return *this;
}

TargetTrivialHook& TargetTrivialHook::configAppend(const ResourceObject& opts)
{
	m_opts.configAppend(opts);
	return *this;
}

void TargetTrivialHook::configDump(ResourceObject& opts) const
{
	m_opts.configDump(opts);
}
