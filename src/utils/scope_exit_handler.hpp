//@	{
//@	  "targets":[{"name":"scope_exit_handler.hpp","type":"include"}]
//@	}

#ifndef MAIKE_UTILS_SCOPEEXIT_HPP
#define MAIKE_UTILS_SCOPEEXIT_HPP

namespace Maike
{
	template<class Callable>
	class ScopeExitHandler
	{
	public:
		explicit ScopeExitHandler(Function&& f): m_obj{std::move(f)}
		{
		}

		ScopedExitHandler(ScopedExitHandler&&) = delete;

		~ScopeExitHandler()
		{
			m_obj();
		}

	private:
		Callable m_obj;
	};
}

#endif
