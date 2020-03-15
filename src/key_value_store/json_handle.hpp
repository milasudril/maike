//@	{
//@	  "targets":[{"name":"json_handle.hpp","type":"include", "pkgconfig_libs":["jansson"]}]
//@	 ,"dependencies_extra":[{"ref":"json_handle.o","rel":"implementation"}]
//@	 }


#ifndef MAIKE_KEYVALUESTORE_JSONHANDLE_HPP
#define MAIKE_KEYVALUESTORE_JSONHANDLE_HPP

#include <jansson.h>

#include <memory>
#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace Maike::KeyValueStore
{
	enum class Type:int
	{
		Object = JSON_OBJECT,
		Array = JSON_ARRAY,
		String = JSON_STRING,
		Integer = JSON_INTEGER,
		Float = JSON_REAL,
		True = JSON_TRUE,
		False = JSON_FALSE,
		Null = JSON_NULL
	};

	constexpr std::string_view name(Type type)
	{
		switch(type)
		{
			case Type::Object:
				return "compound";
			case Type::Array:
				return "array";
			case Type::String:
				return "string";
			case Type::Integer:
				return "integer";
			case Type::Float:
				return "float";
			case Type::True:
				return "true";
			case Type::False:
				return "false";
			case Type::Null:
				return "null";
			default:
				return "unknown";
		}
	}

	class TypeError: public std::runtime_error
	{
	public:
		explicit TypeError(Type expected, Type got, std::string_view src_name):
		   std::runtime_error{std::string{src_name}
			.append(": error: Expected an object of type `")
			.append(name(expected))
			.append("`, got `")
			.append(name(got))
			.append("`.")}
		{
		}
	};

	template<Type expected>
	inline void validateType(Type got, std::string_view src_name)
	{
		if(expected != got)
		{
			throw TypeError{expected, got, src_name};
		}
	}

	class JsonHandle
	{
	public:
		JsonHandle() = default;

		explicit JsonHandle(json_t* handle): m_handle{handle}
		{
		}

		json_t* get()
		{
			return m_handle.get();
		}

		json_t const* get() const
		{
			return m_handle.get();
		}

		bool valid() const
		{
			return m_handle != nullptr;
		}

		Type type() const
		{ return static_cast<Type>(json_typeof(m_handle.get()));}

	private:
		struct JsonDeleter
		{
			void operator()(json_t* handle)
			{
				if(handle != nullptr) { json_decref(handle); }
			}
		};

		std::unique_ptr<json_t, JsonDeleter> m_handle;
	};

	namespace detail
	{
		struct ReadCallbackBase
		{
			bool m_has_data;
		};

		using ReadCallback = json_load_callback_t;
		JsonHandle jsonLoad(ReadCallbackBase* obj, ReadCallback cb, std::string_view src_name);

		bool hasNonWhitespace(std::byte const* begin, std::byte const* end);

		template<class Source>
		size_t fetch(Source& src, std::byte* buffer, size_t n)
		{
			size_t n_written = 0;
			while(n_written != n)
			{
				auto ch_in = getchar(src);
				static_assert(std::is_same_v<std::decay_t<decltype(ch_in)>, int>);
				if (ch_in == -1)
				{ return 0; }

				*buffer = static_cast<std::byte>(ch_in);
				++buffer;
				++n_written;

				if(ch_in == '}' || ch_in == ']' || (ch_in>= ' ' && ch_in<=' '))
				{ return n_written;}
			}
			return n_written;
		}

		template<class Source>
		struct ReadCallbackObj:public ReadCallbackBase
		{
			Source* r_src;
		};
	}

	template<class Source>
	JsonHandle jsonLoad(Source&& src)
	{
		detail::ReadCallbackObj<std::decay_t<Source>> cb{false, &src};

		return detail::jsonLoad(
		   &cb,
		   [](void* buffer, size_t bufflen, void* obj) {
			   auto self = reinterpret_cast<detail::ReadCallbackObj<std::decay_t<Source>>*>(obj);
			   auto buff_bytes = reinterpret_cast<std::byte*>(buffer);
			   auto const ret = detail::fetch(*self->r_src, buff_bytes, bufflen);
			   if(!self->m_has_data)
			   { self->m_has_data = detail::hasNonWhitespace(buff_bytes, buff_bytes + ret);}
			   return ret;
		   },
		   name(src));
	}

	template<class Sink>
	void store(json_t const* handle, Sink&& sink)
	{
		json_dump_callback(
		   handle,
		   [](char const* buffer, size_t bufflen, void* data) {
			   using SelfT = std::decay_t<Sink>;
			   auto& self = *reinterpret_cast<SelfT*>(data);
			   write(self, buffer, bufflen);
			   return 0;
		   },
		   &sink,
		   JSON_SORT_KEYS | JSON_ENCODE_ANY | JSON_INDENT(4));
	}

	template<class Sink>
	void store(JsonHandle const& obj, Sink&& sink)
	{store(obj.get(), sink);}
}

#endif