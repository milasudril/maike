//@	{
//@	 "targets":[{"name":"target_hook_registry.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"target_hook_registry.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGET_HOOK_REGISTRY_HPP
#define MAIKE_TARGET_HOOK_REGISTRY_HPP

#include "visibility.hpp"
#include "handle.hpp"
#include "target_hook.hpp"
#include <map>
#include <vector>

namespace Maike
	{
	class ResourceObject;
	class Target_Hook_Plugin;
	class Stringkey;
	class ParameterSetDumpable;

	class PRIVATE Target_Hook_Registry
		{
		public:
			Target_Hook_Registry(ParameterSetDumpable&& sysvars)=delete;
			Target_Hook_Registry(const ParameterSetDumpable& sysvars);
			~Target_Hook_Registry();
			Target_Hook_Registry& configAppend(const ResourceObject& targethooks);
			Target_Hook_Registry& configAppendDefault();
			void configDump(ResourceObject& targethooks) const;
			void configClear();

			class PRIVATE EnumCallbackFilenameExt
				{
				public:
					virtual void operator()(const Stringkey& filename_ext,const Target_Hook& hook)=0;
				};
			void enumerate(EnumCallbackFilenameExt&& callback) const;

		private:
			const ParameterSetDumpable& r_sysvars;

			static Target_Hook_Plugin& pluginLoad(const char* name);

			static std::map<Stringkey,Target_Hook_Plugin> s_plugins;
			struct PRIVATE HookInfo
				{
				std::string name;
				std::string plugin;
				Handle<Target_Hook> hook;
				};

			Target_Hook& hookCreate(Target_Hook_Plugin& plug,const char* name);

			std::map<Stringkey, HookInfo > m_hooks;
			std::map<Stringkey, const Target_Hook* > r_filenameext_hook;
		};
	}

#endif

