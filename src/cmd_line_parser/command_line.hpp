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
#include <stdexcept>

namespace Maike::CmdLineParser
{
	constexpr char const* typeToString(Empty<fs::path>)
	{
		return "path";
	}

	inline fs::path fromString(Empty<fs::path>, char const* str)
	{
		return Maike::fs::path{str};
	}

	constexpr char const* typeToString(Empty<std::vector<fs::path>>)
	{
		return "paths";
	}

	namespace detail
	{
		std::pair<std::string, char const*> next_array_token(char const* str);
	}

	template<class T>
	std::vector<T> fromString(Empty<std::vector<T>>, char const* str)
	{
		std::vector<T> ret;
		while(true)
		{
			auto [buffer, termpos] = detail::next_array_token(str);
			ret.push_back(fromString(Empty<T>{}, buffer.c_str()));
			if(*termpos == '\0') { return ret; }
			str = termpos;
		}
	}

	inline std::false_type fromString(Empty<std::false_type>, char const* str)
	{
		if(strlen(str) != 0) { throw std::runtime_error{"Option does not take any value"}; }

		return std::false_type{};
	}


	class OptionInfo
	{
	public:
		constexpr OptionInfo():
		   r_category{nullptr},
		   r_name{nullptr},
		   r_type{nullptr},
		   r_summary{nullptr},
		   r_description{nullptr},
		   m_value_req{false}
		{
		}

		template<class EnumItemTraits>
		constexpr explicit OptionInfo(Empty<EnumItemTraits>):
		   r_category{EnumItemTraits::category()},
		   r_name{EnumItemTraits::name()},
		   r_type{typeToString(Empty<typename EnumItemTraits::type>{})},
		   r_summary{EnumItemTraits::summary()},
		   r_description{EnumItemTraits::description()},
		   m_value_req{EnumItemTraits::valueRequired()}
		{
		}

		constexpr auto category() const
		{
			return r_category;
		}

		constexpr auto name() const
		{
			return r_name;
		}

		constexpr auto type() const
		{
			return r_type;
		}

		constexpr auto summary() const
		{
			return r_summary;
		}

		constexpr auto description() const
		{
			return r_description;
		}

		constexpr auto valueRequired() const
		{
			return m_value_req;
		}


	private:
		char const* r_category;
		char const* r_name;
		char const* r_type;
		char const* r_summary;
		char const* r_description;
		bool m_value_req;
	};

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

		using StringToValue = uint64_t (*)(void* tuple, char const* str);

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


		template<class EnumType, template<auto> class EnumItemTraits, int K = end(Empty<EnumType>{})>
		struct GetOptionNames
		{
			static constexpr void setItem(std::array<OptItem, end(Empty<EnumType>{})>& names)
			{
				constexpr auto index = K - 1;
				using Traits = EnumItemTraits<static_cast<EnumType>(index)>;
				names[index] = {
				   Traits::name(), [](void* tuple, char const* str) -> uint64_t {
					   if constexpr(Traits::valueRequired())
					   {
						   if(*str == '\0')
						   {
							   throw std::runtime_error{std::string{"Option `"} + Traits::name() + "` requires a value"};
						   }
					   }
					   using Tuple = TupleFromEnum<EnumType, EnumItemTraits>;
					   auto self = reinterpret_cast<Tuple*>(tuple);
					   set<static_cast<EnumType>(index)>(*self, fromString(Empty<typename Traits::type>{}, str));
					   return 1llu << static_cast<uint64_t>(index);
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

		template<class EnumType>
		constexpr bool check_duplicate_names(std::array<OptItem, end(Empty<EnumType>{})> const& vals)
		{
			auto name_prev = vals[0].name;
			for(size_t k = 1; k < vals.size(); ++k)
			{
				if(strcmp(vals[k].name, name_prev) == 0) { return true; }

				name_prev = vals[k].name;
			}
			return false;
		}

		uint64_t collect_options(char const* const* argv_begin,
		                         char const* const* argv_end,
		                         OptItem const* optitems_begin,
		                         OptItem const* optitems_end,
		                         void* tuple);

		struct CompareOptInfoByCategoryAndName
		{
			constexpr bool operator()(OptionInfo const& a, OptionInfo const& b) const
			{
				auto const diff = strcmp(a.category(), b.category());
				if(diff < 0) { return true; }

				if(diff == 0) { return strcmp(a.name(), b.name()) < 0; }

				return false;
			}
		};

		template<class EnumType, template<auto> class EnumItemTraits, int K = end(Empty<EnumType>{})>
		struct GetOptionInfo
		{
			static constexpr void setItem(std::array<OptionInfo, end(Empty<EnumType>{})>& names)
			{
				constexpr auto index = K - 1;
				using Traits = EnumItemTraits<static_cast<EnumType>(index)>;
				names[index] = OptionInfo{Empty<Traits>{}};
				GetOptionInfo<EnumType, EnumItemTraits, index>::setItem(names);
			}
		};

		template<class EnumType, template<auto> class EnumItemTraits>
		struct GetOptionInfo<EnumType, EnumItemTraits, 0>
		{
			static constexpr void setItem(std::array<OptionInfo, end(Empty<EnumType>{})>&)
			{
			}
		};

		template<class EnumType, template<EnumType> class EnumItemTraits>
		constexpr auto get_option_info()
		{
			std::array<OptionInfo, end(Empty<EnumType>{})> ret{};
			GetOptionInfo<EnumType, EnumItemTraits>::setItem(ret);
			return ret;
		}
	}

	template<class EnumType, template<EnumType> class EnumItemTraits>
	class BasicCommandLine
	{
	public:
		explicit BasicCommandLine(int argc, char** argv): m_set_vals{0}
		{
			if(argc < 1) { return; }

			m_set_vals = detail::collect_options(
			   argv, argv + argc, std::begin(s_option_names), std::end(s_option_names), &m_data);
		}

		template<EnumType index>
		auto const& option() const
		{
			return get<index>(m_data);
		}

		template<EnumType index>
		bool hasOption() const
		{
			return m_set_vals & (1 << static_cast<uint64_t>(index));
		}

		template<class Function>
		void forEachOption(Function&& f) const
		{
			apply(
			   [func = std::forward<Function>(f), this](auto tag, auto const& value) mutable {
				   if(hasOption<tag.value>()) { func(EnumItemTraits<tag.value>::name(), value); }
			   },
			   m_data);
		}

		static constexpr auto optionInfo()
		{
			return s_option_info;
		}

	private:
		uint64_t m_set_vals;
		TupleFromEnum<EnumType, EnumItemTraits> m_data;
		static constexpr auto s_option_names = detail::sort(
		   detail::get_option_names<EnumType, EnumItemTraits>(), detail::CompareOptItemsByName{});

		static constexpr auto s_option_info =
		   detail::sort(detail::get_option_info<EnumType, EnumItemTraits>(),
		                detail::CompareOptInfoByCategoryAndName{});

		static_assert(detail::check_duplicate_names<EnumType>(s_option_names) == false);
	};
}

#endif