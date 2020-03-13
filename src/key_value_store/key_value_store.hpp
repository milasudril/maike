//@	{
//@	  "targets":[{"name":"key_value_store.hpp","type":"include", "pkgconfig_libs":["jansson"]}]
//@	 ,"dependencies_extra":[{"ref":"key_value_store.o", "rel":"implementation"}]
//@	 }

#ifndef MAIKE_KEYVALUESTORE_HPP
#define MAIKE_KEYVALUESTORE_HPP

#include <jansson.h>

#include <stdexcept>
#include <cstddef>
#include <type_traits>

namespace Maike::KeyValueStore
{
	class ObjectRefConst
	{
	public:
		explicit ObjectRefConst(json_t* handle, char const* name): r_handle{handle}, r_name{name}
		{
		}

		template<class T>
		T as() const = delete;

		char const* name() const
		{
			return r_name;
		}

		json_t const* handle() const
		{
			return r_handle;
		}

	private:
		json_t* r_handle;
		char const* r_name;
	};


	template<class T>
	inline T get(ObjectRefConst obj)
	{
		return obj.template as<T>();
	}

	template<class T>
	struct Tag
	{
	};

	template<class T>
	inline T get(Tag<T>, ObjectRefConst obj)
	{
		return get<T>(obj);
	}

	class TypeError: public std::runtime_error
	{
	public:
		explicit TypeError(const char* src, const char* type):
		   std::runtime_error{std::string{src} + ": error: `" + type + "` expected."}
		{
		}
	};

	class KeyLookupError: public std::runtime_error
	{
	public:
		explicit KeyLookupError(char const* src, char const* key):
		   std::runtime_error{std::string{src} + ": error: `" + key
		                      + "` does not exist in the current compound."}
		{
		}
	};

	namespace detail
	{
		template<class T>
		inline T get_adl(Tag<T>, ObjectRefConst obj)
		{
			return get(Tag<T>{}, obj);
		}
	}

	class CompoundRefConst
	{
	public:
		explicit CompoundRefConst(json_t* handle, char const* name): r_handle{handle}, r_name{name}
		{
			if(!json_is_object(handle)) { throw TypeError{name, "Compound"}; }
		}

		template<class T>
		T get(char const* key) const
		{
			auto obj = json_object_get(r_handle, key);
			if(obj == nullptr) { throw KeyLookupError{r_name, key}; }

			return detail::get_adl(Tag<T>{}, ObjectRefConst{obj, r_name});
		}

	private:
		json_t* r_handle;
		char const* r_name;
	};

	class ArrayRefConst
	{
	public:
		class const_iterator
		{
		public:
			using value_type = ObjectRefConst;

			explicit const_iterator(json_t* handle, size_t index, char const* name):
			   r_handle{handle},
			   m_index{index},
			   r_name{name}
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
				return ObjectRefConst{json_array_get(r_handle, m_index), r_name};
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
			json_t* r_handle;
			size_t m_index;
			char const* r_name;
		};

		explicit ArrayRefConst(json_t* handle, char const* name): r_handle{handle}, r_name{name}
		{
			if(!json_is_array(handle)) { throw TypeError{name, "Array"}; }
		}

		const_iterator begin() const
		{
			return const_iterator{r_handle, 0, r_name};
		}

		const_iterator end() const
		{
			return const_iterator{r_handle, json_array_size(r_handle), r_name};
		}

	private:
		json_t* r_handle;
		char const* r_name;
	};

	template<>
	inline char const* ObjectRefConst::as<char const*>() const
	{
		if(!json_is_string(r_handle)) { throw TypeError{r_name, "String"}; }
		return json_string_value(r_handle);
	}

	template<>
	inline json_int_t ObjectRefConst::as<json_int_t>() const
	{
		if(!json_is_integer(r_handle)) { throw TypeError{r_name, "Integer"}; }
		return json_integer_value(r_handle);
	}

	template<>
	inline double ObjectRefConst::as<double>() const
	{
		if(!json_is_real(r_handle)) { throw TypeError{r_name, "Double"}; }
		return json_real_value(r_handle);
	}

	template<>
	inline CompoundRefConst ObjectRefConst::as<CompoundRefConst>() const
	{
		return CompoundRefConst{r_handle, r_name};
	}

	template<>
	inline ArrayRefConst ObjectRefConst::as<ArrayRefConst>() const
	{
		return ArrayRefConst{r_handle, r_name};
	}

	class DecodeError: public std::runtime_error
	{
	public:
		explicit DecodeError(std::string_view source, int line, int col, char const* description);
	};

	class Object
	{
	public:
		Object(): m_handle{json_object()}
		{
		}


		Object(Object&& other): m_handle{other.m_handle}
		{
			other.m_handle = nullptr;
		}

		Object& operator=(Object&& other)
		{
			std::swap(m_handle, other.m_handle);
			json_decref(other.m_handle);
			other.m_handle = nullptr;
			return *this;
		}

		~Object()
		{
			if(m_handle != nullptr) { json_decref(m_handle); }
		}

		template<class T>
		explicit Object(T x, std::enable_if_t<std::is_integral_v<T>, int> = 0):m_handle{json_integer(x)}{}

		explicit Object(char const* str):m_handle{json_string(str)}{}

		explicit Object(double x):m_handle{json_real(x)}{}

		template<class Source>
		explicit Object(Source&& src, std::enable_if_t<!std::is_integral_v<Source>, int> = 0)
		{
			json_error_t err;
			m_handle = json_load_callback(
			   [](void* buffer, size_t bufflen, void* data) {
				   using SelfT = std::decay_t<Source>;
				   auto& self = *reinterpret_cast<SelfT*>(data);
				   return static_cast<size_t>(read(self, reinterpret_cast<std::byte*>(buffer), bufflen));
			   },
			   &src,
			   JSON_DISABLE_EOF_CHECK | JSON_DECODE_ANY,
			   &err);
			if(m_handle == nullptr && !(err.line == 1 && err.column == 0))
			{ throw DecodeError{name(src), err.line, err.column, err.text}; }
			m_name = name(src);
		}

		ObjectRefConst get() const
		{
			return ObjectRefConst{m_handle, m_name.c_str()};
		}

		template<class T>
		auto as() const
		{
			return get().template as<T>();
		}


		bool empty() const
		{
			return m_handle == nullptr;
		}

	private:
		json_t* m_handle;
		std::string m_name;
	};

	template<class Sink>
	void write(Object const& obj, Sink&& sink)
	{
		json_dump_callback(
		   obj.get().handle(),
		   [](char const* buffer, size_t bufflen, void* data) {
			   using SelfT = std::decay_t<Sink>;
			   auto& self = *reinterpret_cast<SelfT*>(data);
			   write(self, buffer, bufflen);
			   return 0;
		   },
		   &sink,
		   JSON_SORT_KEYS | JSON_ENCODE_ANY | JSON_INDENT(4));
	}
}

#endif
