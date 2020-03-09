//@	{
//@	  "targets":[{"name":"config_store.hpp","type":"include", "pkgconfig_libs":["jansson"]}]
//@	 ,"dependencies_extra":[{"ref":"config_store.o", "rel":"implementation"}]
//@	 }

#ifndef MAIKE_CONFIGSTORE_HPP
#define MAIKE_CONFIGSTORE_HPP

#include <jansson.h>

#include <stdexcept>
#include <cstddef>

namespace Maike
{
	class ConfigStore
	{
	public:
		class DecodeError:public std::runtime_error
		{
			public:
				explicit DecodeError(std::string_view source, int line, int col, char const* description);
		};

		ConfigStore():m_root(nullptr){}
		ConfigStore(ConfigStore&&);
		ConfigStore& operator=(ConfigStore&&);
		~ConfigStore()
		{
			if(m_root != nullptr)
			{ json_decref(m_root); }
		}

		template<class Source>
		ConfigStore(Source&& src)
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

		template<class T>
		T const& get(char const* key);

		template<class T>
		ConfigStore& insert(char const* key, T&& value);

		bool empty() const
		{ return m_root == nullptr;}

	private:
		json_t* m_root;
	};
}

#endif
