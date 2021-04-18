//@	{
//@	  "targets":[{"name":"compound.o","type":"object"}]
//@	 }

#include "./compound.hpp"
#include "./array.hpp"

#include <algorithm>

Maike::KeyValueStore::Compound& Maike::KeyValueStore::operator|=(Compound& a, CompoundRefConst b)
{
	std::for_each(std::begin(b), std::end(b), [&a](auto const& item){
		if(auto item_in_a = a.getIfAny(item.first); item_in_a)
		{
			if(item_in_a->type() != item.second.type())
			{
				throw std::runtime_error{"Type mismatch"};
			}

			switch(item_in_a->type())
			{
				case Type::Array:
					a.set(item.first, Array{item_in_a->template as<ArrayRefConst>()}
						| item.second.template as<ArrayRefConst>());
					break;

				case Type::Compound:
					a.set(item.first, Compound{item_in_a->template as<CompoundRefConst>()}
						| item.second.template as<CompoundRefConst>());
					break;

				default:
					a.set(item.first, item.second);
			}
		}
		else
		{
			a.set(item.first, item.second);
		}
	});
	return a;
}