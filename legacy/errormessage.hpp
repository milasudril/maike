//@	{
//@	 "targets":[{"name":"errormessage.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"errormessage.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_ERRORMESSAGE_HPP
#define MAIKE_ERRORMESSAGE_HPP

#include "visibility.hpp"
#include <initializer_list>
#include <cstddef>

namespace Maike
	{
	class Variant;
	class PRIVATE ErrorMessage
		{
		public:
			explicit ErrorMessage(const char* format_string
				,const std::initializer_list<Variant>& args) noexcept;

			static constexpr size_t length()
				{return N;}

			const char* messageGet() const noexcept
				{return m_buffer;}

		private:
			static constexpr size_t N=1024;
			char m_buffer[N];
		};
	}

#endif
