//@	{
//@	 "targets":[{"name":"config_store.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_CONFIGSTORE_HPP
#define MAIKE_CONFIGSTORE_HPP

namespace Maike
{
	class ConfigStore
	{
	public:
		ConfigStore()=default;
		ConfigStore(ConfigStore&&);
		ConfigStore& operator=(ConfigStore&&);
		~ConfigStore();

		template<class T>
		T const& get(char const* key);

		template<class T>
		ConfigStore& insert(char const* key, T&& value);

	private:
	};
}

#endif
