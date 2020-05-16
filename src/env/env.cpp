//@	{
//@	  "targets":[{"name":"env.o","type":"object"}]
//@	}

#include "./env.hpp"

namespace
{
	char const* get_var_end(char const* ptr)
	{
		while(true)
		{
			auto ch_in = *ptr;
			if(ch_in == '\0' || ch_in == '=') { return ptr; }
			++ptr;
		}
	}
}

void Maike::Env::detail::load(void (*do_insert)(void* container,
                                                std::string_view name,
                                                std::string_view value),
                              void* container,
                              char const* const* env)
{
	while(true)
	{
		auto str = *env;
		++env;
		if(str == nullptr) { return; }

		auto i = get_var_end(str);
		switch(*i)
		{
			case '\0':
				do_insert(container, std::string_view{str, static_cast<size_t>(i - str)}, std::string_view{""});
				break;

			case '=':
				do_insert(container, std::string_view{str, static_cast<size_t>(i - str)}, i + 1);
				break;
		}
	}
}

#if 0
#include <memory>
#include <algorithm>

extern "C" char** environ;

Maike::EnvCache Maike::env;



void Maike::EnvCache::reload()
{
	std::map<std::string, std::string, std::less<>> tmp;
	auto ptr = environ;
	while(true)
	{
		auto str = *ptr;
		++ptr;
		if(str == nullptr)
		{
			static_cast<Base&>(*this) = tmp;
			m_dirty = false;
			return;
		}

		auto i = get_var_end(str);
		switch(*i)
		{
			case '\0': tmp.insert(std::pair{std::string{str, static_cast<size_t>(i - str)}, ""}); break;

			case '=': tmp.insert(std::pair{std::string{str, static_cast<size_t>(i - str)}, i + 1}); break;
		}
	}
}

void Maike::EnvCache::updateEnviron()
{
	auto tmp_strings = std::make_unique<std::string[]>(size());
	std::transform(begin(), end(), tmp_strings.get(), [](auto const& item) {
		return item.first + "=" + item.second;
	});

	auto tmp_string_ptrs = std::make_unique<char*[]>(size());
	std::transform(tmp_strings.get(),
	               tmp_strings.get() + size(),
	               tmp_string_ptrs.get(),
	               [](auto& item) { return item.data(); });

	env_strings = std::move(tmp_strings);
	env_string_ptrs = std::move(tmp_string_ptrs);
	environ = env_string_ptrs.get();
	m_dirty = false;
}

extern "C"
{
	// Overides for libc functions manipulating the environment
	char* getenv(char const* name)
	{
		if(Maike::env.dirty()) { Maike::env.updateEnviron(); }

		auto i = Maike::env.find(std::string_view{name});
		return i == Maike::env.end() ? nullptr : i->second.data();
	}

	int putenv(char*)
	{
		fprintf(stderr, "Putenv has been removed, Reason: Not compatible API with EnvCache.");
		abort();
	}

	int setenv(char const* name, char const* value, int overwrite)
	{
		if(overwrite != 0)
		{
			Maike::env[name] = std::string{value};
			return 0;
		}
		(void)Maike::env.insert(std::pair{std::string{name}, value});
		return 0;
	}

	int unsetenv(char const* name)
	{
		Maike::env.erase(std::string_view{name});
		return 0;
	}
}
#endif