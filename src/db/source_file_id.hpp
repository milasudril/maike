//@	{
//@	 }

#ifndef MAIKE_DB_SOURCEFILEID_HPP
#define MAIKE_DB_SOURCEFILEID_HPP

#include <cstddef>
#include <limits>
#include <cassert>

namespace Maike::Db
{
	class SourceFileId
	{
	public:
		constexpr SourceFileId(): m_value{std::numeric_limits<size_t>::max()}
		{
		}

		constexpr explicit SourceFileId(size_t val): m_value{val}
		{
		}

		explicit constexpr operator size_t() const
		{
			assert(valid());
			return m_value;
		}

		constexpr size_t value() const
		{
			assert(valid());
			return m_value;
		}

		constexpr bool valid() const
		{
			return m_value != std::numeric_limits<size_t>::max();
		}

	private:
		size_t m_value;
	};

	constexpr bool operator==(SourceFileId a, SourceFileId b)
	{
		return a.value() == b.value();
	}

	constexpr bool operator!=(SourceFileId a, SourceFileId b)
	{
		return !(a == b);
	}

	constexpr bool valid(SourceFileId id)
	{
		return id.valid();
	}
}

#endif
