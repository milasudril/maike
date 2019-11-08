//@	{"targets":[{"name":"targettrivialhook.o","type":"object"}]}

#include "targettrivialhook.hpp"

using namespace Maike;

TargetTrivialHook* TargetTrivialHook::create()
	{return new TargetTrivialHook();}

void TargetTrivialHook::destroy() noexcept
	{delete this;}

void TargetTrivialHook::configClear()
	{}


TargetTrivialHook& TargetTrivialHook::configAppendDefault()
	{return *this;}

TargetTrivialHook& TargetTrivialHook::configAppend(const ResourceObject&)
	{return *this;}

void TargetTrivialHook::configDump(ResourceObject&) const
	{}
