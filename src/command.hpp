//@	{"targets":[{"name":"command.hpp","type":"include"}]}

#ifndef MAIKE_COMMAND_HPP
#define MAIKE_COMMAND_HPP

#include "./fs.hpp"
#include "src/key_value_store/compound.hpp"
#include "src/key_value_store/array.hpp"

#include <string>
#include <vector>

namespace Maike
{
	class Command
	{
	public:
		explicit Command(fs::path&& exe, std::vector<std::string>&& args):
		   m_executable{std::move(exe)},
		   m_args{std::move(args)}
		{
		}

		explicit Command(KeyValueStore::CompoundRefConst obj)
		{
			m_executable = obj.get<char const*>("executable");
			auto args_kvs = obj.get<KeyValueStore::ArrayRefConst>("args");
			m_args.reserve(std::size(args_kvs));
			std::transform(std::begin(args_kvs),
			               std::end(args_kvs),
			               std::back_inserter(m_args),
			               [](auto const& item) { return item.template as<char const*>(); });
		}

		fs::path const& executable() const
		{
			return m_executable;
		}

		std::vector<std::string> const& args() const
		{
			return m_args;
		}

		template<class Invoker, class IoRedirectorType>
		decltype(auto) invoke(Invoker const& invoker, IoRedirectorType&& redir) const
		{
			return execp(invoker, m_executable, m_args, std::forward<IoRedirectorType>(redir));
		}

	private:
		fs::path m_executable;
		std::vector<std::string> m_args;
	};

	bool operator==(Command const& a, Command const& b)
	{
		return a.executable() == b.executable() && a.args() == b.args();
	}

	bool operator!=(Command const& a, Command const& b)
	{
		return !(a == b);
	}

	inline auto toJson(Command const& cmd)
	{
		auto const& args = cmd.args();
		return KeyValueStore::Compound{}
		   .set("executable", cmd.executable().c_str())
		   .set("args", KeyValueStore::Array{std::begin(args), std::end(args)})
		   .takeHandle();
	}

	inline auto fromJson(KeyValueStore::Empty<Command>, KeyValueStore::JsonRefConst ref)
	{
		return Command{ref.as<KeyValueStore::CompoundRefConst>()};
	}
}

#endif
