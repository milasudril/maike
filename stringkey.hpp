//@	{
//@	"targets":[{"name":"stringkey.hpp","type":"include"}]
//@	}

#ifndef MAIKE_STRINGKEY_HPP
#define MAIKE_STRINGKEY_HPP

#include <cstdint>

namespace Maike
	{
	class Stringkey
		{
		public:
			typedef uint64_t HashValue;

			explicit constexpr Stringkey(const char* str) noexcept:
				m_value(hash(str))
				{}

			constexpr bool operator<(const Stringkey& key) const noexcept
				{return m_value < key.m_value;}

			constexpr bool operator==(const Stringkey& key) const noexcept
				{return key.m_value==m_value;}

			constexpr bool operator!=(const Stringkey& key) const noexcept
				{return !(key==*this);}

			constexpr operator HashValue() const noexcept
				{return m_value;}

		private:
			HashValue m_value;

			static constexpr uint64_t OFFSET_BASIS=0xcbf29ce484222325;
			static constexpr uint64_t FNV_PRIME=0x100000001b3;
			static constexpr uint64_t hash(const char* str
				,HashValue value_init=OFFSET_BASIS)
				{
				return *str=='\0'?
					 value_init
					:hash(str+1, (value_init^(HashValue(*str)))*FNV_PRIME);
				}
		};
	}

#endif

