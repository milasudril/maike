//@	{
//@	  "targets":[{"name":"compound.hpp","type":"include"}]
//@	 }


#ifndef MAIKE_KEYVALUESTORE_COMPOUND_HPP
#define MAIKE_KEYVALUESTORE_COMPOUND_HPP

#include "./json_handle.hpp"

namespace Maike::KeyValueStore
{
	class Compound
	{
		public:
			template<class Source>
			explicit Compound(Source&& src):m_handle{jsonLoad(std::forward<Source>(src))}
			{
				if(m_handle.valid())
				{
					if(m_handle.type() != JsonHandle::Type::Object)
					{ throw "Type error"; }
				}
			}

			Compound():m_handle{json_object()}
			{}

			template<class T>
			Compound& set(char const* key, T value);

			template<class T>
			T get(char const* key);

			size_t size() const
			{ return json_object_size(m_handle.get());}

		private:
			JsonHandle m_handle;
	};
}

#endif