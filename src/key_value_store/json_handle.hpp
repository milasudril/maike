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

namespace Maike::KeyValueStore
{
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
		using ReadCallback = ssize_t (*)(void* obj, char* buffer, size_t n);
		JsonHandle jsonLoad(void* obj, ReadCallback cb, std::string_view src_name);
	}

	template<class Source>
	JsonHandle jsonLoad(Source&& src)
	{
		return detail::jsonLoad(
		   &src,
		   [](void* obj, char* buffer, size_t n) {
			   using SelfT = std::decay_t<Source>;
			   auto& self = *reinterpret_cast<SelfT*>(obj);
			   return static_cast<ssize_t>(read(self, reinterpret_cast<std::byte*>(buffer), n));
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
}

#endif