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


	class ObjectRef
	{
	public:
		explicit ObjectRef(json_t* handle): r_handle{handle}
		{
		}

		json_t* handle() const
		{
			return r_handle;
		}

		template<class T>
		void set(T value);

	private:
		json_t* r_handle;
	};

	template<>
	inline void ObjectRef::set<char const*>(char const* val)
	{
		json_string_set(r_handle, val);
	}

	template<>
	inline void ObjectRef::set<json_int_t>(json_int_t val)
	{
		json_integer_set(r_handle, val);
	}

	template<>
	inline void ObjectRef::set<double>(double val)
	{
		json_real_set(r_handle, val);
	}


	class DecodeError: public std::runtime_error
	{
	public:
		explicit DecodeError(std::string_view source, int line, int col, char const* description);
	};

	class Object
	{
	public:
		Object(): m_root{json_object()}
		{
		}

		Object(Object&& other): m_root{other.m_root}
		{
			other.m_root = nullptr;
		}

		Object& operator=(Object&& other)
		{
			std::swap(m_root, other.m_root);
			json_decref(other.m_root);
			other.m_root = nullptr;
			return *this;
		}

		~Object()
		{
			if(m_root != nullptr) { json_decref(m_root); }
		}

		template<class Source>
		Object(Source&& src)
		{
			json_error_t err;
			m_root = json_load_callback(
			   [](void* buffer, size_t bufflen, void* data) {
				   using SelfT = std::decay_t<Source>;
				   auto& self = *reinterpret_cast<SelfT*>(data);
				   return static_cast<size_t>(read(self, reinterpret_cast<std::byte*>(buffer), bufflen));
			   },
			   &src,
			   JSON_DISABLE_EOF_CHECK,
			   &err);
			if(m_root == nullptr && !(err.line == 1 && err.column == 0))
			{ throw DecodeError{name(src), err.line, err.column, err.text}; }
			m_name = name(src);
		}

		ObjectRefConst get() const
		{
			return ObjectRefConst{m_root, m_name.c_str()};
		}

		ObjectRef get()
		{
			return ObjectRef{m_root};
		}

		bool empty() const
		{
			return m_root == nullptr;
		}

	private:
		json_t* m_root;
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
		   JSON_SORT_KEYS | JSON_INDENT(4));
	}
}

#endif
