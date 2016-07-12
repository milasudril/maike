//@	{"targets":[{"name":"targetcxxhook.o","type":"object"}]}

#include "targetcxxhook.hpp"

using namespace Maike;

TargetCxxHook::TargetCxxHook(const ResourceObject& cxxoptions
	,const ParameterSet& params):m_options(cxxoptions)
	,m_compiler(m_options,params),m_loader(m_options),m_factory(m_compiler)
	{
	}

TargetCxxHook* TargetCxxHook::create(const ResourceObject& cxxoptions
	,const ParameterSet& params)
	{return new TargetCxxHook(cxxoptions,params);}

void TargetCxxHook::destroy() noexcept
	{delete this;}

void TargetCxxHook::configClear()
	{m_options.clear();}

TargetCxxHook& TargetCxxHook::configAppend(const ResourceObject& cxxoptions)
	{
	m_options.configAppend(cxxoptions);
	return *this;
	}
