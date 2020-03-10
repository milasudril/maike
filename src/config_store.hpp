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
	class ConfigObjectRefConst
	{
		public:
			explicit ConfigObjectRefConst(json_t* handle): m_handle{handle}{}

			template<class T>
			T get(char const* key) const;

			template<class T>
			T as() const;

		private:
			json_t* m_handle;
	};

	class ConfigObjectArrayRefConst
	{
		public:
			class const_iterator
			{
				public:
					using value_type = ConfigObjectRefConst;


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
					{ return ConfigObjectRefConst{json_array_get(m_handle, m_index)};}

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

			explicit ConfigObjectArrayRefConst(json_t* handle): m_handle{handle}
			{}

			const_iterator begin() const
			{ return const_iterator{m_handle, 0};}

			const_iterator end() const
			{ return const_iterator{m_handle, json_array_size(m_handle)};}

		private:
			json_t* m_handle;
	};

	template<>
	inline char const* ConfigObjectRefConst::as<char const*>() const
	{
		return json_string_value(m_handle);
	}

	template<>
	inline ConfigObjectRefConst ConfigObjectRefConst::get<ConfigObjectRefConst>(char const* key) const
	{
		return ConfigObjectRefConst{json_object_get(m_handle, key)};
	}

	template<>
	inline char const* ConfigObjectRefConst::get<char const*>(char const* key) const
	{
		return get<ConfigObjectRefConst>(key).as<char const*>();
	}

	template<>
	inline ConfigObjectArrayRefConst ConfigObjectRefConst::get<ConfigObjectArrayRefConst>(char const* key) const
	{
		return ConfigObjectArrayRefConst{json_object_get(m_handle, key)};
	}

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

		ConfigObjectRefConst get() const
		{ return ConfigObjectRefConst{m_root};}

		bool empty() const
		{ return m_root == nullptr;}

	private:
		json_t* m_root;
	};
}

#endif
