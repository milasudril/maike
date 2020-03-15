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

	class Compound;

	JsonHandle toJson(Compound compound);

	class CompoundRefConst
	{
	public:
		explicit CompoundRefConst(JsonRefConst ref): m_ref{ref}
		{
			validateType<Type::Compound>(ref.type(), ref.source());
		}

		template<class T>
		T get(char const* key) const
		{
			auto obj = json_object_get(m_ref.handle(), key);
			if(obj == nullptr) { throw KeyLookupError{m_ref.source(), key}; }
			return JsonRefConst{obj, m_ref.source()}.as<T>();
		}

		size_t size() const
		{
			return json_object_size(m_ref.handle());
		}

	private:
		JsonRefConst m_ref;
	};

	class Compound
	{
	public:
		template<class Source>
		explicit Compound(Source&& src): m_handle{jsonLoad(src)}
		{
			if(m_handle.valid()) { validateType<Type::Compound>(m_handle.type(), name(src)); }
		}

		Compound(): m_handle{json_object()}
		{
		}

		template<class T>
		Compound& set(char const* key, T&& value) &
		{
			(void)json_object_set_new(m_handle.handle(), key, toJson(std::forward<T>(value)).release());
			return *this;
		}

		template<class T>
		Compound&& set(char const* key, T&& value) &&
		{
			(void)json_object_set_new(m_handle.handle(), key, toJson(std::forward<T>(value)).release());
			return std::move(*this);
		}

		template<class T>
		T get(char const* key) const
		{
			auto obj = json_object_get(m_handle.handle(), key);
			if(obj == nullptr) { throw KeyLookupError{m_handle.source(), key}; }
			return JsonRefConst{obj, m_handle.source().c_str()}.as<T>();
		}

		size_t size() const
		{
			return json_object_size(m_handle.handle());
		}

		JsonHandle takeHandle()
		{
			return std::move(m_handle);
		}

		JsonRefConst reference() const
		{
			return m_handle.reference();
		}

	private:
		JsonHandle m_handle;
	};

	inline JsonHandle toJson(Compound compound)
	{
		return compound.takeHandle();
	}

	inline CompoundRefConst fromJson(Empty<CompoundRefConst>, JsonRefConst ref)
	{
		return CompoundRefConst{ref};
	}

	template<class Sink>
	void store(Compound const& obj, Sink&& sink)
	{
		store(obj.reference(), sink);
	}
}

#endif