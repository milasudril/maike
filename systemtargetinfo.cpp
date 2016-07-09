//@	{"targets":[{"name":"systemtargetinfo.o","type":"object"}]}

#include "systemtargetinfo.hpp"
#include "resourceobject.hpp"
#include "sysvars.hpp"
#include "stringkey.hpp"
#include "variant.hpp"
#include "stringformat.hpp"
#include "mapreplace.hpp"

using namespace Maike;

SystemTargetInfo::SystemTargetInfo(const ResourceObject& config)
	{
	sysvarsLoad(m_sysvars,m_strings,m_varnames);
		{
		Stringkey key("target_directory");
		auto i=replace(m_strings,{key,std::string("__targets")});
		replace(m_sysvars,{key,i->second.c_str()});
		}
	auto i=config.objectIteratorGet();
	while(!i.endAt())
		{
		auto p=i.get();
		auto key=Stringkey(p.first);
		m_varnames[key]=std::string(p.first);
		switch(p.second.typeGet())
			{
			case ResourceObject::Type::ARRAY:
				replace(m_sysvars,{key,"[...]"});
				break;
			case ResourceObject::Type::FLOAT:
				replace(m_sysvars,{key,static_cast<double>(p.second)});
				break;
			case ResourceObject::Type::INTEGER:
				replace(m_sysvars,{key,static_cast<int64_t>(static_cast<long long int>(p.second))});
				break;
			case ResourceObject::Type::OBJECT:
				replace(m_sysvars,{key,"{...}"});
				break;
			case ResourceObject::Type::STRING:
				{
				auto str=static_cast<const char*>(p.second);
				auto i=replace(m_strings,{key,std::string(str)});
				replace(m_sysvars,{key,i->second.c_str()});
				}
				break;
			}
		i.next();
		}
	}

SystemTargetInfo::~SystemTargetInfo()
	{
	}

void SystemTargetInfo::parameterGet(const Stringkey& key
	,ParameterProcessor&& proc) const
	{
	auto i=m_sysvars.find(key);
	if(i!=m_sysvars.end())
		{
		auto& var=i->second;
		if(var.typeGet()==Variant::STRING)
			{proc(static_cast<const char*>(var));}
		else
			{
			char buffer[32];
			format({buffer,buffer+32},"#0;",{var});
			proc(buffer);
			}
		}
	}

Variant SystemTargetInfo::variableGet(const Stringkey& key) const noexcept
	{
	auto i=m_sysvars.find(key);
	if(i==m_sysvars.end())
		{return static_cast<int64_t>(0);}
	return i->second;
	}
