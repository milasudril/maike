//@	{"targets":[{"name":"mapreplace.hpp","type":"include"}]}

#include "visibility.hpp"
#include <map>

namespace Maike
	{
//C++17 This function can be replaced by insert_or_assign
	template<class Key,class Value>
	PRIVATE typename std::map<Key,Value>::iterator replace(std::map<Key,Value>& m,std::pair<Key,Value>&& value)
		{
		auto i=m.find(value.first);
		if(i==m.end())
			{return m.insert(i,std::move(value));}
		i->second=std::move(value.second);
		return i;
		}
	}
