//@	{
//@	 "targets":[{"name":"build_id.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"build_id.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_BUILDID_HPP
#define MAIKE_BUILDID_HPP

#include "src/key_value_store/json_handle.hpp"

#include <array>

namespace Maike
{
	class BuildId
	{
	public:
		BuildId();

		explicit BuildId(std::array<std::byte, 32> const& bytes): m_bytes{bytes}
		{
		}

		auto const& bytes() const
		{
			return m_bytes;
		}

	private:
		std::array<std::byte, 32> m_bytes;
	};

	inline bool operator==(BuildId const& a, BuildId const& b)
	{
		return a.bytes() == b.bytes();
	}

	inline bool operator!=(BuildId const& a, BuildId const& b)
	{
		return !(a == b);
	}

	std::string toString(BuildId const& id);

	inline auto toJson(BuildId const& id)
	{
		return KeyValueStore::JsonHandle{toString(id).c_str()};
	}
}

#endif