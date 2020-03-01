//@	{"targets":[{"name":"targetcphook.o","type":"object"}]}

#include "targetcphook.hpp"

using namespace Maike;

TargetCPHook* TargetCPHook::create()
{
	return new TargetCPHook();
}

void TargetCPHook::destroy() noexcept
{
	delete this;
}

void TargetCPHook::configClear()
{
}


TargetCPHook& TargetCPHook::configAppendDefault()
{
	return *this;
}

TargetCPHook& TargetCPHook::configAppend(const ResourceObject&)
{
	return *this;
}

void TargetCPHook::configDump(ResourceObject&) const
{
}
