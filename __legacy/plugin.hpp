//@	{
//@	 "targets":[{"name":"plugin.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"plugin.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_PLUGIN_HPP
#define MAIKE_PLUGIN_HPP

#include "visibility.hpp"
#include <string>

namespace Maike
{
	class PRIVATE Plugin
	{
	public:
		Plugin(const Plugin&) = delete;
		Plugin& operator=(const Plugin&) = delete;

		explicit Plugin(const char* name);
		~Plugin() noexcept;
		Plugin(Plugin&& obj) noexcept;
		Plugin& operator=(Plugin&& obj) noexcept;

		bool dead() const noexcept;

		const char* nameFullGet() const noexcept
		{
			return m_name_full.c_str();
		}

		const char* nameGet() const noexcept
		{
			return m_name.c_str();
		}

	protected:
		template<class T>
		T entryPointGet(const char* name) const
		{
			return reinterpret_cast<T>(functionGet(name));
		}

	private:
		typedef void (*Function)();

		Function functionGet(const char* name) const;

		std::string m_name_full;
		std::string m_name;
		void* m_handle;
	};
}

#endif
