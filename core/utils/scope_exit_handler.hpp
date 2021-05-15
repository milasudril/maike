//@	{
//@	}

#ifndef MAIKE_UTILS_SCOPEEXIT_HPP
#define MAIKE_UTILS_SCOPEEXIT_HPP

namespace Maike
{
	template<class Callable>
	class ScopeExitHandler
	{
	public:
		explicit ScopeExitHandler(Callable&& f): m_obj{std::move(f)}
		{
		}

		ScopeExitHandler(ScopeExitHandler&&) = delete;

		~ScopeExitHandler()
		{
			m_obj();
		}

	private:
		Callable m_obj;
	};
}

#endif
