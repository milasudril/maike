//@	{
//@	  "targets":[{"name":"json_handle.hpp","type":"include", "pkgconfig_libs":["jansson"]}]
//@	 }


#ifndef MAIKE_KEYVALUESTORE_JSONHANDLE_HPP
#define MAIKE_KEYVALUESTORE_JSONHANDLE_HPP

#include <jansson.h>

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
			return m_handle;
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
	}
}

#endif