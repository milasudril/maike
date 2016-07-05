//@	{"targets":[{"name":"handle.hpp","type":"include"}]}

#ifndef MAIKE_HANDLE_HPP
#define MAIKE_HANDLE_HPP

#include <memory>
#include <cassert>

namespace Maike
	{
	template<class T>
	class Handle:public std::unique_ptr<T,void (*)(typename T::Base*)>
		{
		public:
			typedef std::unique_ptr<T,void (*)(typename T::Base*)> Base;

			explicit Handle(T* handle):
			std::unique_ptr<T,void(*)(typename T::Base*)>(handle,T::Base::destroy)
				{
				assert(Base::get()!=nullptr);
				}

			operator Handle<typename T::Base>() noexcept
				{
				auto ptr=Base::release();
				assert(ptr!=nullptr);
				return Handle<typename T::Base>(ptr);
				}
		};
	}

#endif
