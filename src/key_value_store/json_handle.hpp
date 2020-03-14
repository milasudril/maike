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
	{store(obj.get(), std::forward<Sink>(sink));}
}

#endif