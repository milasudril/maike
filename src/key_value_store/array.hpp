//@	{
//@	 "dependencies_extra":[{"ref":"./array.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_KEYVALUESTORE_ARRAY_HPP
#define MAIKE_KEYVALUESTORE_ARRAY_HPP

#include "./json_handle.hpp"

#include <type_traits>
#include <algorithm>

namespace Maike::KeyValueStore
{
	class Array;

	JsonHandle toJson(Array array);

	class ArrayRefConst
	{
	public:
		class const_iterator
		{
		public:
			using value_type = JsonRefConst;

			explicit const_iterator(json_t const* handle, size_t index, char const* name):
			   r_handle{handle}, m_index{index}, r_name{name}
			{
			}

			const_iterator& operator++()
			{
				++m_index;
				return *this;
			}

			const_iterator operator++(int)
			{
				auto ret = *this;
				++m_index;
				return ret;
			}

			value_type operator*() const
			{
				return value_type{json_array_get(r_handle, m_index), r_name};
			}

			bool operator==(const_iterator other) const
			{
				return r_handle == other.r_handle && m_index == other.m_index;
			}

			bool operator!=(const_iterator other) const
			{
				return !(*this == other);
			}

		private:
			json_t const* r_handle;
			size_t m_index;
			char const* r_name;
		};

		explicit ArrayRefConst(JsonRefConst ref): m_ref{ref}
		{
			validateType<Type::Array>(ref.type(), ref.source());
		}

		const_iterator begin() const
		{
			return const_iterator{m_ref.handle(), 0, m_ref.source()};
		}

		const_iterator end() const
		{
			return const_iterator{m_ref.handle(), size(), m_ref.source()};
		}

		size_t size() const
		{
			return json_array_size(m_ref.handle());
		}

		auto handle() const
		{
			return m_ref;
		}

	private:
		JsonRefConst m_ref;
	};

	class Array
	{
	public:
		template<class Source, std::enable_if_t<!std::is_same_v<Array, std::decay_t<Source>>, int> = 0>
		explicit Array(Source&& src, std::string_view src_name): m_handle{jsonLoad(src, src_name)}
		{
			if(m_handle.valid()) { validateType<Type::Array>(m_handle.type(), src_name); }
		}

		Array(): m_handle{json_array()}
		{
		}

		explicit Array(ArrayRefConst ref): m_handle{ref.handle()}
		{
		}

		template<class Iter>
		explicit Array(Iter a, Iter b): Array{}
		{
			std::for_each(a, b, [this](auto&& val) {
				using T = decltype(val);
				append(std::forward<T>(val));
			});
		}

		JsonHandle takeHandle()
		{
			return std::move(m_handle);
		}

		template<class T>
		Array& append(T&& value) &
		{
			(void)json_array_append_new(m_handle.handle(), toJson(std::forward<T>(value)).release());
			return *this;
		}

		template<class T>
		Array&& append(T&& value) &&
		{
			(void)json_array_append_new(m_handle.handle(), toJson(std::forward<T>(value)).release());
			return std::move(*this);
		}

		size_t size() const
		{
			return json_array_size(m_handle.handle());
		}

		auto begin() const
		{
			return ArrayRefConst::const_iterator{handle(), 0, m_handle.source().c_str()};
		}

		auto end() const
		{
			return ArrayRefConst::const_iterator{handle(), size(), m_handle.source().c_str()};
		}

		// FIXME: Should return ArrayRefConst
		JsonRefConst reference() const
		{
			return m_handle.reference();
		}

		json_t const* handle() const
		{
			return m_handle.handle();
		}

	private:
		JsonHandle m_handle;
	};

	inline JsonHandle toJson(Array array)
	{
		return array.takeHandle();
	}

	inline ArrayRefConst fromJson(Empty<ArrayRefConst>, JsonRefConst ref)
	{
		return ArrayRefConst{ref};
	}

	template<class Sink>
	void store(Array const& obj, Sink&& sink)
	{
		store(obj.reference(), sink);
	}

	Array& operator|=(Array& a, ArrayRefConst b);

	inline Array operator|(Array const& a, ArrayRefConst b) = delete;

	inline Array operator|(Array&& a, ArrayRefConst b)
	{
		return a |= b;
	}
}

#endif
