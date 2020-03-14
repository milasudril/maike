//@	{
//@	  "targets":[{"name":"json_handle.hpp","type":"include", "pkgconfig_libs":["jansson"]}]
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

	class DecodeError: public std::runtime_error
	{
	public:
		explicit DecodeError(std::string_view source,
                                               int line,
                                               int col,
                                               char const* description):
   std::runtime_error{std::string{source} + ':' + std::to_string(line) + ':' + std::to_string(col + 1) + ": "
		       + description + '.'}
		       {}
	};

	template<class Source>
	JsonHandle jsonLoad(Source&& src)
	{
		json_error_t err{};
		auto handle = json_load_callback(
		   [](void* buffer, size_t bufflen, void* data) {
			   using SelfT = std::decay_t<Source>;
			   auto& self = *reinterpret_cast<SelfT*>(data);
			   return static_cast<size_t>(read(self, reinterpret_cast<std::byte*>(buffer), bufflen));
		   },
		   &src,
		   JSON_DISABLE_EOF_CHECK | JSON_DECODE_ANY,
		   &err);

		if(handle == nullptr && !(err.line == 1 && err.column == 0))
		{ throw DecodeError{name(src), err.line, err.column, err.text}; }

		return JsonHandle{handle};
	}

	template<class Sink>
	void store(json_t const* handle, Sink&& sink)
	{
		json_dump_callback(handle,
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