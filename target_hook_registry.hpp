//@	{
//@	 "targets":[{"name":"target_hook_registry.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"target_hook_registry.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGET_HOOK_REGISTRY_HPP
#define MAIKE_TARGET_HOOK_REGISTRY_HPP

#include "visibility.hpp"
#include "handle.hpp"
#include <map>
#include <vector>

namespace Maike
	{
	class ResourceObject;
	class Target_Hook_Plugin;
	class Stringkey;
	class Target_Hook;
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

			class PRIVATE EnumCallback
				{
				public:
					virtual void operator()(const Stringkey& filename_ext,const Target_Hook& hook)=0;
				};
			void enumerate(EnumCallback&& callback) const;

		private:
			const ParameterSetDumpable& r_sysvars;

			static Target_Hook_Plugin& pluginLoad(const char* name);

			static std::map<Stringkey,Target_Hook_Plugin> s_plugins;
			std::map<Stringkey, Handle<Target_Hook> > m_hooks;
			std::map<Stringkey, const Target_Hook* > r_filenameext_hook;
			std::map<Stringkey,std::string> m_stringmap;
		};
	}

#endif

