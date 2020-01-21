//@	{
//@	 "targets":[{"name":"target_hook_plugin.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"target_hook_plugin.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGET_HOOK_PLUGIN_HPP
#define MAIKE_TARGET_HOOK_PLUGIN_HPP

#include "plugin.hpp"
#include "target_hook.hpp"
#include "handle.hpp"

namespace Maike
	{
	class PRIVATE Target_Hook_Plugin:private Plugin
		{
		public:
			Target_Hook_Plugin(const char* filename);

			Target_Hook_Plugin(Target_Hook_Plugin&&)=default;
			Target_Hook_Plugin& operator=(Target_Hook_Plugin&&)=default;

			~Target_Hook_Plugin()
				{
				if(!dead())
					{m_cleanup();}
				}

			Handle<Target_Hook> create(const Maike::ParameterSetDumpable& params)
				{return Handle<Target_Hook>( m_hook_create(params) );}

			using Plugin::nameGet;
			using Plugin::nameFullGet;

		private:
			decltype(&Maike_Plugin_cleanup) m_cleanup;
			decltype(&Maike_Plugin_init) m_init;
			decltype(&Maike_Target_Hook_create) m_hook_create;
		};
	}

#endif
