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
			explicit Compound(Source&& src):m_handle{jsonLoad(src)}
			{
				if(m_handle.valid())
				{
					validateType<Type::Object>(m_handle.type(), name(src));
				}
			}

			Compound():m_handle{json_object()}
			{}

			template<class T>
			Compound& set(char const* key, T value);

			template<class T>
			T get(char const* key) const;

			size_t size() const
			{ return json_object_size(m_handle.get());}

		private:
			JsonHandle m_handle;
	};

	template<>
	char const* Compound::get<char const*>(char const* key) const
	{
		auto obj = json_object_get(m_handle.get(), key);
		validateType<Type::String>(static_cast<Type>(json_typeof(obj)), "");

		return json_string_value(obj);
	}
}

#endif