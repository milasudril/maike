//@	{"targets":[{"name":"target_hook_registry.o","type":"object"}]}

#include "target_hook_registry.hpp"
#include "resourceobject.hpp"
#include "target_hook_plugin.hpp"
#include "stringkey.hpp"
#include "mapreplace.hpp"

using namespace Maike;

std::map<Stringkey,Target_Hook_Plugin> Target_Hook_Registry::s_plugins;

Target_Hook_Plugin& Target_Hook_Registry::pluginLoad(const char* name)
	{
	auto key=Stringkey(name);
	auto i=s_plugins.find(key);
	if(i==s_plugins.end())
		{
		return s_plugins.emplace(std::move(key),Target_Hook_Plugin(name)).first
			->second;
		}
	return i->second;
	}

Target_Hook_Registry::Target_Hook_Registry(const ParameterSetDumpable& sysvars):
	r_sysvars(sysvars)
	{}

Target_Hook_Registry::~Target_Hook_Registry()
	{}

Target_Hook_Registry& Target_Hook_Registry::configAppend(const ResourceObject& targethooks)
	{
	auto N=targethooks.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		auto obj=targethooks.objectGet(k);
		auto plugin_name=static_cast<const char*>(obj.objectGet("plugin"));
		auto& plugin=pluginLoad(plugin_name);

		auto hook_name=static_cast<const char*>(obj.objectGet("name"));
		auto& hook=replace(m_hooks,{Stringkey(hook_name),plugin.create(r_sysvars)})
			->second;

		auto filename_exts=obj.objectGet("filename_exts");
		auto M=filename_exts.objectCountGet();
		for(decltype(M) l=0;l<M;++l)
			{
			auto fext=static_cast<const char*>(filename_exts.objectGet(l));
			r_filenameext_hook[Stringkey(fext)]=hook.get();
			}

		hook->configAppend(obj.objectGet("config"));
		}

	return *this;
	}
