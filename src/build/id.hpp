//@	{
//@	 "targets":[{"name":"id.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"id.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_BUILD_ID_HPP
#define MAIKE_BUILD_ID_HPP

#include "src/utils/empty.hpp"
#include "src/key_value_store/json_handle.hpp"

#include <array>

namespace Maike::Build
{
	class Id
	{
	public:
		Id();

		explicit Id(char const* cstr);

		explicit Id(std::array<std::byte, 32> const& bytes): m_bytes{bytes}
		{
		}

		auto const& bytes() const
		{
			return m_bytes;
		}

	private:
		std::array<std::byte, 32> m_bytes;
	};

	inline bool operator==(Id const& a, Id const& b)
	{
		return a.bytes() == b.bytes();
	}

	inline bool operator!=(Id const& a, Id const& b)
	{
		return !(a == b);
	}

	std::string toString(Id const& id);

	inline auto toJson(Id const& id)
	{
		return KeyValueStore::JsonHandle{toString(id).c_str()};
	}

	inline auto fromString(Empty<Id>, char const* str)
	{
		return Id{str};
	}

	constexpr inline char const* typeToString(Empty<Id>)
	{
		return "BuildId";
	}
}

#endif