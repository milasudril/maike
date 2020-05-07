//@	{
//@	  "targets":[{"name":"enum_tuple.hpp","type":"include"}]
//@	}

#ifndef MAIKE_CMDLINEPARSER_ENUMTUPLE_HPP
#define MAIKE_CMDLINEPARSER_ENUMTUPLE_HPP

#include "src/empty.hpp"

#include <utility>

namespace Maike::CmdLineParser
{
	namespace detail
	{
		template<int Val, class EnumType, template<EnumType> class EnumItemTraits>
		struct MakeTupleFromEnum: public MakeTupleFromEnum<Val - 1, EnumType, EnumItemTraits>
		{
			typename EnumItemTraits<static_cast<EnumType>(Val - 1)>::type m_value;
		};

		template<class EnumType, template<auto> class T>
		struct MakeTupleFromEnum<0, EnumType, T>
		{
		};

		template<class T>
		constexpr auto endAsT()
		{
			return static_cast<T>(end(Empty<T>{}));
		}
	}

	template<class EnumType, template<auto> class EnumItemTraits>
	using TupleFromEnum = detail::MakeTupleFromEnum<end(Empty<EnumType>{}), EnumType, EnumItemTraits>;

	template<auto EnumType, template<auto> class EnumItemTraits>
	constexpr auto const& get(TupleFromEnum<decltype(EnumType), EnumItemTraits> const& x)
	{
		return static_cast<detail::MakeTupleFromEnum<static_cast<int>(EnumType) + 1,
		                                             decltype(EnumType),
		                                             EnumItemTraits> const&>(x)
		   .m_value;
	}

	template<auto EnumType, template<auto> class EnumItemTraits, class T>
	constexpr void set(TupleFromEnum<decltype(EnumType), EnumItemTraits>& x, T&& val)
	{
		static_cast<detail::MakeTupleFromEnum<static_cast<int>(EnumType) + 1,
		                                      decltype(EnumType),
		                                      EnumItemTraits>&>(x)
		   .m_value = std::forward<T>(val);
	}
}
#endif