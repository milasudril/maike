//@	{
//@	 "targets":[{"name":"invoker.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DB_INVOKER_HPP
#define MAIKE_DB_INVOKER_HPP

#include "src/exec/result.hpp"
#include "src/exec/command.hpp"

namespace Maike::Db
{
	namespace detail
	{
		template<class... Args>
		inline decltype(auto) do_execve(Args&&... args)
		{
			return execve(std::forward<Args>(args)...);
		}
	}
	class Invoker
	{
	public:
		template<class T>
		explicit Invoker(std::reference_wrapper<T> obj):
		   m_handle{&obj.get()}, m_func{[](void* handle, Exec::Command const& cmd) {
			   auto& self = *static_cast<T*>(handle);
			   return detail::do_execve(self, cmd);
		   }}
		{
		}

		Exec::Result execve(Exec::Command const& cmd)
		{
			return m_func(m_handle, cmd);
		}

	private:
		void* m_handle;
		Exec::Result (*m_func)(void* handle, Exec::Command const& cmd);
	};
}

#endif