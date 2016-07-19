//@	{"targets":[{"name":"target_hook_plugin.o","type":"object"}]}

#include "target_hook_plugin.hpp"
#include "stdstream.hpp"
#include "exceptionhandler.hpp"


using namespace Maike;

Target_Hook_Plugin::Target_Hook_Plugin(const char* filename):Plugin(filename)
	{
	m_cleanup=entryPointGet<decltype(m_cleanup)>("Maike_Plugin_cleanup");
	m_init=entryPointGet<decltype(m_init)>("Maike_Plugin_init");
	m_hook_create=entryPointGet<decltype(m_hook_create)>("Maike_Target_Hook_create");
	m_init(StdStream::output(),StdStream::error(),exceptionHandlerGet());
	}
