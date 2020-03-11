//@	{
//@	  "targets":[{"name":"key_value_store.hpp","type":"include", "pkgconfig_libs":["jansson"]}]
//@	 ,"dependencies_extra":[{"ref":"key_value_store.o", "rel":"implementation"}]
//@	 }

#ifndef MAIKE_KEYVALUESTORE_HPP
#define MAIKE_KEYVALUESTORE_HPP

#include <jansson.h>

#include <stdexcept>
#include <cstddef>

namespace Maike::KeyValueStore
{
	class ObjectRefConst
	{
		public:
			explicit ObjectRefConst(json_t* handle): m_handle{handle}{}

			template<class T>
			T as() const = delete;

		private:
			json_t* m_handle;
	};

	class CompoundRefConst
	{
	public:
		explicit CompoundRefConst(json_t* handle): m_handle{handle}{}

		template<class T>
		T get(char const* key) const
		{ return ObjectRefConst{json_object_get(m_handle, key)}.template as<T>();}

		private:
			json_t* m_handle;
	};

	class ArrayRefConst
	{
		public:
			class const_iterator
			{
				public:
					using value_type = ObjectRefConst;


					explicit const_iterator(json_t* handle, size_t index): m_handle{handle}, m_index{index}
					{}

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
					{ return ObjectRefConst{json_array_get(m_handle, m_index)};}

					bool operator==(const_iterator other) const
					{
						return m_handle == other.m_handle && m_index == other.m_index;
					}

					bool operator!=(const_iterator other) const
					{
						return !(*this == other);
					}

				private:
					json_t* m_handle;
					size_t m_index;
			};

			explicit ArrayRefConst(json_t* handle): m_handle{handle}
			{}

			const_iterator begin() const
			{ return const_iterator{m_handle, 0};}

			const_iterator end() const
			{ return const_iterator{m_handle, json_array_size(m_handle)};}

		private:
			json_t* m_handle;
	};

	template<>
	inline char const* ObjectRefConst::as<char const*>() const
	{
		return json_string_value(m_handle);
	}

	template<>
	inline json_int_t ObjectRefConst::as<json_int_t>() const
	{
		return json_integer_value(m_handle);
	}

	template<>
	inline double ObjectRefConst::as<double>() const
	{
		return json_real_value(m_handle);
	}

	template<>
	inline CompoundRefConst ObjectRefConst::as<CompoundRefConst>() const
	{
		return CompoundRefConst{m_handle};
	}

	template<>
	inline ArrayRefConst ObjectRefConst::as<ArrayRefConst>() const
	{
		return ArrayRefConst{m_handle};
	}

	template<class T>
	inline T get(ObjectRefConst obj)
	{
		return obj.template as<T>();
	}

	template<class T>
	struct Tag
	{};

	template<class T>
	inline T get(Tag<T>, ObjectRefConst obj)
	{
		return get<T>(obj);
	}

	class DecodeError:public std::runtime_error
	{
		public:
			explicit DecodeError(std::string_view source, int line, int col, char const* description);
	};

	class Object
	{
	public:
		Object():m_root(nullptr){}
		Object(Object&&);
		Object& operator=(Object&&);
		~Object()
		{
			if(m_root != nullptr)
			{ json_decref(m_root); }
		}

		template<class Source>
		Object(Source&& src)
		{
			json_error_t err;
			m_root = json_load_callback([](void* buffer, size_t bufflen, void* data){
				using SelfT = std::decay_t<Source>;
				auto& self = *reinterpret_cast<SelfT*>(data);
				return static_cast<size_t>( read(self, reinterpret_cast<std::byte*>(buffer), bufflen) );
			}, &src, JSON_DISABLE_EOF_CHECK, &err);
			if(m_root == nullptr && !(err.line == 1 && err.column == 0))
			{
				throw DecodeError{name(src), err.line, err.column, err.text};
			}
		}

		ObjectRefConst get() const
		{ return ObjectRefConst{m_root};}

		bool empty() const
		{ return m_root == nullptr;}

	private:
		json_t* m_root;
	};
}

#endif
