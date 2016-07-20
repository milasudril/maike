//@	{"targets":[{"name":"target_hook_registry.o","type":"object"}]}

#include "target_hook_registry.hpp"
#include "resourceobject.hpp"
#include "target_hook_plugin.hpp"
#include "stringkey.hpp"

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

Target_Hook& Target_Hook_Registry::hookCreate(Target_Hook_Plugin& plug,const char* name)
	{
	auto key=Stringkey(name);
	auto i=m_hooks.find(key);
	if(i==m_hooks.end())
		{
		HookInfo hi{name,plug.nameGet(),plug.create(r_sysvars)};
		return *m_hooks.emplace(std::move(key),std::move(hi)).first->second.hook.get();
		}
	return *i->second.hook.get();
	}

Target_Hook_Registry& Target_Hook_Registry::configAppend(const ResourceObject& targethooks)
	{
	auto N=targethooks.objectCountGet();
	for(decltype(N) k=0;k<N;++k)
		{
		auto obj=targethooks.objectGet(k);
		auto plugin_name=static_cast<const char*>(obj.objectGet("plugin"));
		auto& plugin=pluginLoad(plugin_name);

		auto hook_name=static_cast<const char*>(obj.objectGet("name"));
		auto& hook=hookCreate(plugin,hook_name);

		auto filename_exts=obj.objectGet("filename_exts");
		auto M=filename_exts.objectCountGet();
		for(decltype(M) l=0;l<M;++l)
			{
			auto fext=static_cast<const char*>(filename_exts.objectGet(l));
			r_filenameext_hook[Stringkey(fext)]=&hook;
			}

		hook.configAppend(obj.objectGet("config"));
		}

	return *this;
	}

Target_Hook_Registry& Target_Hook_Registry::configAppendDefault()
	{
		{
		auto& plug=pluginLoad("targetcxx");
		auto& hook=hookCreate(plug,"cxxdefault");
		r_filenameext_hook[Stringkey(".cpp")]=&hook;
		r_filenameext_hook[Stringkey(".cxx")]=&hook;
		r_filenameext_hook[Stringkey(".c++")]=&hook;
		r_filenameext_hook[Stringkey(".cc")]=&hook;
		r_filenameext_hook[Stringkey(".hpp")]=&hook;
		r_filenameext_hook[Stringkey(".hxx")]=&hook;
		r_filenameext_hook[Stringkey(".h++")]=&hook;
		r_filenameext_hook[Stringkey(".hh")]=&hook;
		r_filenameext_hook[Stringkey(".h")]=&hook;
		hook.configAppendDefault();
		}

		{
		auto& plug=pluginLoad("targetpython");
		auto& hook=hookCreate(plug,"pythondefault");
		r_filenameext_hook[Stringkey(".py")]=&hook;
		hook.configAppendDefault();
		}
	return *this;
	}

void Target_Hook_Registry::configClear()
	{
	r_filenameext_hook.clear();
	m_hooks.clear();
	}

void Target_Hook_Registry::enumerate(EnumCallbackFilenameExt&& callback) const
	{
	auto i=r_filenameext_hook.begin();
	auto i_end=r_filenameext_hook.end();
	while(i!=i_end)
		{
		callback(i->first,*(i->second));
		++i;
		}
	}

void Target_Hook_Registry::configDump(ResourceObject& targethooks) const
	{
	auto i=m_hooks.begin();
	auto i_end=m_hooks.end();
	while(i!=i_end)
		{
		ResourceObject hook(ResourceObject::Type::OBJECT);

		hook.objectSet("name",ResourceObject(i->second.name.c_str()))
			.objectSet("plugin",ResourceObject(i->second.plugin.c_str()));

		ResourceObject config(ResourceObject::Type::OBJECT);
		i->second.hook->configDump(config);
		hook.objectSet("config",std::move(config));

		targethooks.objectAppend(std::move(hook));
		++i;
		}
	}
