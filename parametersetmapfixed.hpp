//@	{
//@	 "targets":[{"name":"parametersetmapfixed.hpp","type":"include"}]
//@	}

#ifndef MAIKE_PARAMETERSETMAPFIXED_HPP
#define MAIKE_PARAMETERSETMAPFIXED_HPP

#include "parameterset.hpp"
#include "stringkey.hpp"
#include "mapfixed.hpp"
#include <vector>
#include <string>

namespace Maike
	{
	template<Stringkey::HashValue ... args>
	class ParameterSetMapFixed : public ParameterSet
		,private MapFixed<Stringkey::HashValue,std::vector<std::string>,args...>
		{
		public:
			typedef MapFixed<Stringkey::HashValue,std::vector<std::string>,args...> BaseType;

			using BaseType::operator[];
			using BaseType::get;
			using BaseType::find;
			using BaseType::size;
			using BaseType::length;
			using BaseType::end;

			void parameterGet(const Stringkey& key,ParameterProcessor&& proc) const;
		};

	template<Stringkey::HashValue ... args>
	void ParameterSetMapFixed<args...>::parameterGet(const Stringkey& key
		,ParameterProcessor&& proc) const
		{
		auto i=find(key);
		if(i!=end())
			{
			auto& v=(*this)[i];
			auto ptr=v.data();
			auto ptr_end=ptr + v.size();
			while(ptr!=ptr_end)
				{
				proc(ptr->c_str());
				++ptr;
				}
			}
		}
	}

#endif
