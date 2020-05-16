//@	{
//@	  "targets":[{"name":"env.hpp","type":"include"}]
//@	  ,"dependencies_extra":[{"ref":"env.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_ENV_HPP
#define MAIKE_ENV_HPP

#include <string_view>
#include <string>
#include <memory>
#include <type_traits>
#include <algorithm>

namespace Maike::Env
{
	namespace detail
	{
		void load(void (*do_insert)(void* container, std::string_view name, std::string_view value),
		          void* container,
		          char const* const* env);
	}

	template<class AssociativeContainer>
	AssociativeContainer load(char const* const* env)
	{
		AssociativeContainer ret;
		detail::load(
		   [](void* container, std::string_view name, std::string_view value) {
			   using Key = typename AssociativeContainer::key_type;
			   auto& self = *reinterpret_cast<AssociativeContainer*>(container);
			   self.insert_or_assign(Key{name}, value);
		   },
		   &ret,
		   env);
		return ret;
	}

	class StringPointers
	{
	public:
		template<
		   class AssociativeContainer,
		   std::enable_if_t<!std::is_same_v<std::decay_t<AssociativeContainer>, StringPointers>, int> = 0>
		explicit StringPointers(AssociativeContainer const& container)
		{
			m_pointers =
			   std::make_unique<std::unique_ptr<char[]>[]>(container.size() + 1); // Last should be nullptr
			std::transform(
			   std::begin(container), std::end(container), m_pointers.get(), [](auto const& item) {
				   auto ret = std::make_unique<char[]>(std::size(item.first) + std::size(item.second) + 2);
				   auto pos = std::copy(std::begin(item.first), std::end(item.first), ret.get());
				   *pos = '=';
				   std::copy(std::begin(item.second), std::end(item.second), pos + 1);
				   return ret;
			   });
		}

		char const* const* get() const
		{
			return reinterpret_cast<char const* const*>(m_pointers.get());
		}

	private:
		std::unique_ptr<std::unique_ptr<char[]>[]> m_pointers;
	};
}

#endif