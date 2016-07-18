//@	{
//@	 "targets":[{"name":"pluginloader.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"pluginloader.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_PLUGINLOADER_HPP
#define MAIKE_PLUGINLOADER_HPP

#include "visibility.hpp"
#include <string>

namespace Maike
	{
	class PRIVATE PluginLoader
		{
		public:
			PluginLoader(const PluginLoader&)=delete;
			PluginLoader& operator=(const PluginLoader&)=delete;

			explicit PluginLoader(const char* name);
			~PluginLoader() noexcept;
			PluginLoader(PluginLoader&& obj) noexcept;
			PluginLoader& operator=(PluginLoader&& obj) noexcept;

			template<class T>
			T entryPointGet(const char* name) const
				{return reinterpret_cast<T>(functionGet(name));}

		private:
			typedef void (*Function)();

			Function functionGet(const char* name) const;

			std::string m_name;
			void* m_handle;
		};
	}

#endif
