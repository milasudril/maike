//@	{
//@	 "targets":[{"name": "command.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"command.o","rel":"implementation"}]
//@	}

#include "src/invoker.hpp"

#include <string>
#include <vector>
#include <variant>

namespace Maike::CommandInterpreter
{
	/* Basic syntax
	 *
	 *
	 * Use ~ as escape symbol
	 *
	 * system.cat(foo.cpp)|system.g++(-, -x, c++, -std=c++17, {system.pkg-config(--libs,gtk+-3)/~ },
	 * -o, foo.o)
	 */

	using CommandOutput = std::vector<std::byte>;

	using EvaluatedArgument = std::vector<std::string>;

	class Literal
	{
	public:
		Literal() = default;

		explicit Literal(std::string&& val): m_value{std::move(val)}
		{
		}

		Literal& value(std::string&& val)
		{
			m_value = val;
			return *this;
		}

		std::string const& value() const
		{
			return m_value;
		}

	private:
		std::string m_value;
	};

	inline bool operator==(Literal const& a, Literal const& b)
	{
		return a.value() == b.value();
	}

	inline bool operator!=(Literal const& a, Literal const& b)
	{
		return !(a == b);
	}

	CommandOutput expand(Literal const& obj, CommandOutput const& sysin);

	inline EvaluatedArgument makeEvaluatedArgument(Literal const& obj)
	{
		return EvaluatedArgument{obj.value()};
	}

	class ExpandString;

	class Command
	{
	public:
		explicit Command(std::string&& name): m_name{std::move(name)}
		{
		}

		std::string const& name() const
		{
			return m_name;
		}
		std::vector<std::variant<Literal, ExpandString>> const& args() const
		{
			return m_args;
		}

		Command& add(Literal&& arg);

		Command& add(ExpandString&& str);

		auto& back()
		{
			return m_args.back();
		}

	private:
		std::string m_name;
		std::vector<std::variant<Literal, ExpandString>> m_args;
	};

	bool operator==(Command const& a, Command const& b);

	inline bool operator!=(Command const& a, Command const& b)
	{
		return !(a == b);
	}

	CommandOutput expand(Command const&, CommandOutput const& sysin);

	class Command;

	class Pipe
	{
	public:
		auto const& commands() const
		{
			return m_commands;
		}

		Pipe& operator|=(Command&& cmd)
		{
			m_commands.push_back(std::move(cmd));
			return *this;
		}

		Pipe& operator|=(Literal&& x)
		{
			m_commands.push_back(std::move(x));
			return *this;
		}

		auto& back()
		{
			return m_commands.back();
		}

	private:
		std::vector<std::variant<Literal, Command>> m_commands;
	};

	inline bool operator==(Pipe const& a, Pipe const& b)
	{
		return a.commands() == b.commands();
	}

	inline bool operator!=(Pipe const& a, Pipe const& b)
	{
		return !(a == b);
	}

	inline Pipe& operator|(Pipe& a, Command&& cmd)
	{
		return a |= std::move(cmd);
	}

	inline Pipe& operator|(Pipe& a, Literal&& x)
	{
		return a |= std::move(x);
	}

	CommandOutput expand(Pipe const& pipe, CommandOutput const& sysin);


	class CommandSplitOutput
	{
	public:
		CommandSplitOutput(): m_separator{'\0'}
		{
		}

		CommandSplitOutput& pipe(Pipe&& pipe)
		{
			m_pipe = std::move(pipe);
			return *this;
		}

		Pipe const& pipe() const
		{
			return m_pipe;
		}

		CommandSplitOutput& separator(char sep)
		{
			m_separator = sep;
			return *this;
		}

		char separator() const
		{
			return m_separator;
		}

	private:
		Pipe m_pipe;
		char m_separator;
	};

	inline bool operator==(CommandSplitOutput const& a, CommandSplitOutput const& b)
	{
		return a.pipe() == b.pipe() && a.separator() == b.separator();
	}

	inline bool operator!=(CommandSplitOutput const& a, CommandSplitOutput const& b)
	{
		return !(a == b);
	}

	EvaluatedArgument makeEvaluatedArgument(CommandSplitOutput const& obj);

	class ExpandString
	{
	public:
		explicit ExpandString(Literal&& prefix): m_prefix{std::move(prefix)}
		{
		}

		Literal const& prefix() const
		{
			return m_prefix;
		}

		CommandSplitOutput const& command() const
		{
			return m_command;
		}

		CommandSplitOutput& command()
		{
			return m_command;
		}

		Literal const& suffix() const
		{
			return m_suffix;
		}

		ExpandString& suffix(Literal&& suffix)
		{
			m_suffix = std::move(suffix);
			return *this;
		}

	private:
		Literal m_prefix;
		CommandSplitOutput m_command;
		Literal m_suffix;
	};

	inline bool operator==(ExpandString const& a, ExpandString const& b)
	{
		return a.prefix() == b.prefix() && a.command() == b.command() && a.suffix() == b.suffix();
	}

	inline bool operator!=(ExpandString const& a, ExpandString const& b)
	{
		return !(a == b);
	}

	inline Command& Command::add(Literal&& arg)
	{
		m_args.push_back(std::move(arg));
		return *this;
	}

	inline Command& Command::add(ExpandString&& str)
	{
		m_args.push_back(std::move(str));
		return *this;
	}

	EvaluatedArgument makeEvaluatedArgument(ExpandString const& obj);

	std::pair<Pipe, char const*> makePipe(char const* buffer);
}