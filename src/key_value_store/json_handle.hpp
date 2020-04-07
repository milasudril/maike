//@	{
//@	  "targets":[{"name":"json_handle.hpp","type":"include", "pkgconfig_libs":["jansson"]}]
//@	 ,"dependencies_extra":[{"ref":"json_handle.o","rel":"implementation"}]
//@	 }


#ifndef MAIKE_KEYVALUESTORE_JSONHANDLE_HPP
#define MAIKE_KEYVALUESTORE_JSONHANDLE_HPP

#include "src/reader.hpp"

#include <jansson.h>

#include <memory>
#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <cassert>

namespace Maike::KeyValueStore
{
	enum class Type : int
	{
		Compound = JSON_OBJECT,
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
			case Type::Compound: return "compound";
			case Type::Array: return "array";
			case Type::String: return "string";
			case Type::Integer: return "integer";
			case Type::Float: return "float";
			case Type::True: return "true";
			case Type::False: return "false";
			case Type::Null: return "null";
			default: return "unknown";
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
		if(expected != got) { throw TypeError{expected, got, src_name}; }
	}

	template<class T>
	struct Empty
	{
	};

	class JsonRefConst
	{
	public:
		explicit JsonRefConst(json_t const* handle, char const* src): r_handle{handle}, r_src{src}
		{
			assert(handle != nullptr);
			assert(src != nullptr);
		}

		template<class T>
		T as() const
		{
			return fromJson(Empty<T>{}, *this);
		}

		Type type() const
		{
			return static_cast<Type>(json_typeof(r_handle));
		}

		json_t const* handle() const
		{
			return r_handle;
		}

		char const* source() const
		{
			return r_src;
		}

	private:
		json_t const* r_handle;
		char const* r_src;
	};

	template<>
	inline char const* JsonRefConst::as() const
	{
		validateType<Type::String>(type(), r_src);
		return json_string_value(r_handle);
	}

	template<>
	inline json_int_t JsonRefConst::as() const
	{
		validateType<Type::Integer>(type(), r_src);
		return json_integer_value(r_handle);
	}

	template<>
	inline double JsonRefConst::as() const
	{
		validateType<Type::Float>(type(), r_src);
		return json_real_value(r_handle);
	}

	template<class T>
	inline T get(JsonRefConst ref)
	{
		return ref.as<T>();
	}

	template<class T>
	inline T get(Empty<T>, JsonRefConst ref)
	{
		return get<T>(ref);
	}


	class JsonHandle
	{
	public:
		JsonHandle() = default;

		template<class IntegralType>
		explicit JsonHandle(IntegralType x,
		                    std::enable_if_t<std::is_integral_v<IntegralType>, std::true_type> = {}):
		   JsonHandle{json_integer(x)}
		{
		}

		explicit JsonHandle(double x): JsonHandle{json_real(x)}
		{
		}

		explicit JsonHandle(char const* x): JsonHandle{json_string(x)}
		{
		}

		explicit JsonHandle(json_t* handle, std::string_view src = "<scratch>"):
		   m_handle{handle},
		   m_src{src}
		{
		}

		json_t* handle()
		{
			return m_handle.get();
		}

		json_t const* handle() const
		{
			return m_handle.get();
		}

		bool valid() const
		{
			return m_handle != nullptr;
		}

		Type type() const
		{
			return static_cast<Type>(json_typeof(m_handle.get()));
		}

		template<class T>
		T as() const
		{
			return reference().as<T>();
		}

		std::string const& source() const
		{
			return m_src;
		}

		auto release()
		{
			return m_handle.release();
		}

		JsonRefConst reference() const
		{
			return JsonRefConst{handle(), m_src.c_str()};
		}

	private:
		struct JsonDeleter
		{
			void operator()(json_t* handle)
			{
				if(handle != nullptr) { json_decref(handle); }
			}
		};

		std::unique_ptr<json_t, JsonDeleter> m_handle;
		std::string m_src;
	};

	template<class T>
	inline T get(JsonHandle const& h)
	{
		return h.as<T>();
	}

	template<class T>
	inline T get(Empty<T>, JsonHandle const& h)
	{
		return get<T>(h);
	}

	template<class T>
	inline JsonHandle toJson(T const& x)
	{
		return JsonHandle{x};
	}

	namespace detail
	{
		struct ReadCallbackBase
		{
			bool m_has_data;
		};

		using ReadCallback = json_load_callback_t;


		bool hasNonWhitespace(std::byte const* begin, std::byte const* end);
	}

	JsonHandle jsonLoad(Reader reader, std::string_view src_name);

	template<class Source>
	JsonHandle jsonLoad(Source&& src)
	{
		return jsonLoad(Reader{src}, name(src));
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
	{
		store(obj.handle(), sink);
	}

	template<class Sink>
	void store(JsonRefConst obj, Sink&& sink)
	{
		store(obj.handle(), sink);
	}
}

#endif