//@	{
//@	  "targets":[{"name":"compound.hpp","type":"include"}]
//@	 }


#ifndef MAIKE_KEYVALUESTORE_COMPOUND_HPP
#define MAIKE_KEYVALUESTORE_COMPOUND_HPP

#include "./json_handle.hpp"

namespace Maike::KeyValueStore
{
	class KeyLookupError: public std::runtime_error
	{
	public:
		explicit KeyLookupError(std::string const& src, char const* key):
		   std::runtime_error{src + ": error: `" + key + "` does not exist in the current compound."}
		{
		}
	};

	class Compound
	{
	public:
		template<class Source>
		explicit Compound(Source&& src): m_handle{jsonLoad(src)}
		{
			if(m_handle.valid()) { validateType<Type::Object>(m_handle.type(), name(src)); }
		}

		Compound(): m_handle{json_object()}
		{
		}

		template<class T>
		Compound& set(char const* key, T value);

		template<class T>
		T get(char const* key) const
		{
			auto obj = json_object_get(m_handle.get(), key);
			if(obj == nullptr) { throw KeyLookupError{m_handle.source(), key}; }
			return JsonRefConst{obj, m_handle.source().c_str()}.as<T>();
		}

		size_t size() const
		{
			return json_object_size(m_handle.get());
		}

	private:
		JsonHandle m_handle;
	};
}

#endif