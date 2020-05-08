//@	{
//@	  "targets":[{"name":"command_line.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"command_line.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_CMDLINEPARSER_COMMANDLINE_HPP
#define MAIKE_CMDLINEPARSER_COMMANDLINE_HPP

#include "./enum_tuple.hpp"
#include "src/fs.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <variant>

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

		using StringToValue = void (*)(void* tuple, char const* str);

		struct OptItem
		{
			char const* name;
			StringToValue converter;
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

		inline std::false_type fromString(Empty<std::false_type>, char const* str)
		{
			if(strlen(str) != 0) { throw std::runtime_error{"Option does not take any value"}; }

			return std::false_type{};
		}

		inline Maike::fs::path fromString(Empty<Maike::fs::path>, char const* str)
		{
			return Maike::fs::path{str};
		}


		template<class EnumType, template<auto> class EnumItemTraits, int K = end(Empty<EnumType>{})>
		struct GetOptionNames
		{
			static constexpr void setItem(std::array<OptItem, end(Empty<EnumType>{})>& names)
			{
				constexpr auto index = K - 1;
				using Traits = EnumItemTraits<static_cast<EnumType>(index)>;
				names[index] = {Traits::name(), [](void* tuple, char const* str) {
					                using Tuple = TupleFromEnum<EnumType, EnumItemTraits>;
					                auto self = reinterpret_cast<Tuple*>(tuple);
					                set<static_cast<EnumType>(index)>(
					                   *self, fromString(Empty<typename Traits::type>{}, str));
				                }};
				GetOptionNames<EnumType, EnumItemTraits, index>::setItem(names);
			}
		};

		template<class EnumType, template<auto> class EnumItemTraits>
		struct GetOptionNames<EnumType, EnumItemTraits, 0>
		{
			static constexpr void setItem(std::array<OptItem, end(Empty<EnumType>{})>&)
			{
			}
		};

		template<class EnumType, template<EnumType> class EnumItemTraits>
		constexpr auto get_option_names()
		{
			std::array<OptItem, end(Empty<EnumType>{})> ret{};
			GetOptionNames<EnumType, EnumItemTraits>::setItem(ret);
			return ret;
		}

		void collect_options(char const* const* argv_begin,
		                     char const* const* argv_end,
		                     OptItem const* optitems_begin,
		                     OptItem const* optitems_end,
		                     void* tuple);
	}

	template<class EnumType, template<EnumType> class EnumItemTraits>
	class BasicCommandLine
	{
	public:
		explicit BasicCommandLine(int argc, char** argv)
		{
			if(argc < 2) { return; }

			detail::collect_options(
			   argv + 1, argv + argc, std::begin(s_option_names), std::end(s_option_names), &m_data);
		}

		template<EnumType index>
		auto const& getopt() const
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