//@	{
//@	  "targets":[{"name":"compound.hpp","type":"include"}]
//@	 }


#ifndef MAIKE_KEYVALUESTORE_COMPOUND_HPP
#define MAIKE_KEYVALUESTORE_COMPOUND_HPP

#include "./json_handle.hpp"

#include <optional>

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

	JsonHandle toJson(Compound&& compound);
	JsonHandle toJson(Compound const& compound);

	class CompoundRefConst
	{
	public:
		class const_iterator
		{
		public:
			using value_type = std::pair<char const* const, JsonRefConst>;

			explicit const_iterator(json_t const* handle, nullptr_t): r_handle{handle}, r_iter{nullptr}
			{
			}

			explicit const_iterator(json_t const* handle, char const* name):
			   r_handle{handle}, r_iter{json_object_iter(const_cast<json_t*>(handle))}, r_name{name}
			{
			}

			const_iterator& operator++()
			{
				r_iter = json_object_iter_next(const_cast<json_t*>(r_handle), r_iter);
				return *this;
			}

			const_iterator operator++(int)
			{
				auto ret = *this;
				r_iter = json_object_iter_next(const_cast<json_t*>(r_handle), r_iter);
				return ret;
			}

			value_type operator*() const
			{
				return value_type{json_object_iter_key(r_iter),
				                  JsonRefConst{json_object_iter_value(r_iter), r_name}};
			}

			bool operator==(const_iterator other) const
			{
				return r_handle == other.r_handle && r_iter == other.r_iter;
			}

			bool operator!=(const_iterator other) const
			{
				return !(*this == other);
			}

		private:
			json_t const* r_handle;
			void* r_iter;
			char const* r_name;
		};

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

		template<class T>
		std::optional<T> getIf(char const* key) const
		{
			auto obj = json_object_get(m_ref.handle(), key);
			if(obj == nullptr) { return std::optional<T>{}; }
			return JsonRefConst{obj, m_ref.source()}.as<T>();
		}

		size_t size() const
		{
			return json_object_size(m_ref.handle());
		}

		const_iterator begin() const
		{
			return const_iterator{m_ref.handle(), m_ref.source()};
		}

		const_iterator end() const
		{
			return const_iterator{m_ref.handle(), nullptr};
		}

	private:
		JsonRefConst m_ref;
	};

	class Compound
	{
	public:
		template<class Source>
		explicit Compound(Source&& src, std::string_view src_name): m_handle{jsonLoad(src, src_name)}
		{
			if(m_handle.valid()) { validateType<Type::Compound>(m_handle.type(), src_name); }
			else
			{
				m_handle = JsonHandle{json_object()};
			}
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

		template<class T>
		std::optional<T> getIf(char const* key) const
		{
			auto obj = json_object_get(m_handle.handle(), key);
			if(obj == nullptr) { return std::optional<T>{}; }
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

		auto reference() const
		{
			return CompoundRefConst{m_handle.reference()};
		}

		auto handleReference() const
		{
			return m_handle.reference();
		}

		auto begin() const
		{
			return reference().begin();
		}

		auto end() const
		{
			return reference().end();
		}

	private:
		JsonHandle m_handle;
	};

	inline JsonHandle toJson(Compound&& compound)
	{
		return std::move(compound).takeHandle();
	}

	inline JsonHandle toJson(Compound const& compound)
	{
		return toJson(Compound{compound});
	}

	inline CompoundRefConst fromJson(Empty<CompoundRefConst>, JsonRefConst ref)
	{
		return CompoundRefConst{ref};
	}

	template<class Sink>
	void store(Compound const& obj, Sink&& sink)
	{
		store(obj.handleReference(), sink);
	}
}

#endif