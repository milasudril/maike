//@	{"targets":[{"name":"parametersetdefault.o","type":"object"}]}

#include "parametersetdefault.hpp"
#include "stringkey.hpp"

using namespace Maike;

Maike::ParameterSetDefault::ParameterSetDefault()
	{}

Maike::ParameterSetDefault::~ParameterSetDefault()
	{}

void ParameterSetDefault::parameterGet(const Stringkey& key,ParameterProcessor&& proc) const
	{
	auto i=m_values.find(key);
	if(i!=m_values.end())
		{
		auto& v=i->second;
		auto str=v.data();
		auto str_end=str + v.size();
		while(str!=str_end)
			{
			proc(str->c_str());
			++str;
			}
		}
	}

void ParameterSetDefault::clear() noexcept
	{
	m_values.clear();
	}
