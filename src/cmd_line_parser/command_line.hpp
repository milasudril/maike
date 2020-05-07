//@	{
//@	  "targets":[{"name":"command_line.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"command_line.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_CMDLINEPARSER_COMMANDLINE_HPP
#define MAIKE_CMDLINEPARSER_COMMANDLINE_HPP

#include "./enum_tuple.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>

namespace Maike::CmdLineParser
{
	namespace detail
	{
		template<class T, size_t N, class Compare>
		constexpr std::array<T, N> sort(std::array<T, N> vals, Compare cmp)
		{
			for(size_t k = 1; k < N; ++k)
			{
				for(size_t l = k; l > 0 && cmp(vals[l], vals[l - 1]); --l)
				{
					auto tmp = vals[l];
					vals[l] = vals[l - 1];
					vals[l - 1] = tmp;
				}
			}
			return vals;
		}

		struct OptItem
		{
			char const* name;
			int index;
		};

		class CompareOptItemsByName
		{
		public:
			constexpr explicit CompareOptItemsByName(size_t str_maxlen = std::numeric_limits<size_t>::max()):
			   m_str_maxlen{str_maxlen}
			{
			}

			constexpr bool operator()(OptItem const& a, OptItem const& b)
			{
				return strncmp(a.name, b.name, m_str_maxlen) < 0;
			}

			constexpr bool operator()(OptItem const& a, char const* b)
			{
				return strncmp(a.name, b, m_str_maxlen) < 0;
			}

			constexpr bool operator()(char const* a, OptItem const& b)
			{
				return strncmp(a, b.name, m_str_maxlen) < 0;
			}

		private:
			size_t m_str_maxlen;
		};

		template<class EnumType, template<auto> class EnumItemTraits, int K = end(Empty<EnumType>{})>
		struct GetOptionNames
		{
			static constexpr void set(std::array<OptItem, end(Empty<EnumType>{})>& names)
			{
				names[K - 1] = {EnumItemTraits<static_cast<EnumType>(K - 1)>::name(), K - 1};
				GetOptionNames<EnumType, EnumItemTraits, K - 1>::set(names);
			}
		};

		template<class EnumType, template<auto> class EnumItemTraits>
		struct GetOptionNames<EnumType, EnumItemTraits, 0>
		{
			static constexpr void set(std::array<OptItem, end(Empty<EnumType>{})>&)
			{
			}
		};

		template<class EnumType, template<EnumType> class EnumItemTraits>
		constexpr auto get_option_names()
		{
			std::array<OptItem, end(Empty<EnumType>{})> ret{};
			GetOptionNames<EnumType, EnumItemTraits>::set(ret);
			return ret;
		}

		void collect_options(char const* const* argv_begin,
		                     char const* const* argv_end,
		                     OptItem const* optitems_begin,
		                     OptItem const* optitems_end);
	}

	template<class EnumType, template<EnumType> class EnumItemTraits>
	class BasicCommandLine
	{
	public:
		explicit BasicCommandLine(int argc, char** argv)
		{
			if(argc < 2) { return; }

			detail::collect_options(
			   argv + 1, argv + argc, std::begin(s_option_names), std::end(s_option_names));
		}

		template<EnumType index>
		auto const& get() const
		{
			return get<index>(m_data);
		}

	private:
		TupleFromEnum<EnumType, EnumItemTraits> m_data;
		static constexpr auto s_option_names = detail::sort(
		   detail::get_option_names<EnumType, EnumItemTraits>(), detail::CompareOptItemsByName{});
	};
}

#endif