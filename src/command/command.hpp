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

	CommandOutput execute(Literal const& obj, CommandOutput const& sysin);

	inline EvaluatedArgument makeEvaluatedArgument(Literal const& obj)
	{
		return EvaluatedArgument{obj.value()};
	}


	class Varname
	{
	public:
		Varname() = default;

		explicit Varname(std::string&& val): m_value{std::move(val)}
		{
		}

		Varname& value(std::string&& val)
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

	inline bool operator==(Varname const& a, Varname const& b)
	{
		return a.value() == b.value();
	}

	inline bool operator!=(Varname const& a, Varname const& b)
	{
		return !(a == b);
	}

	// TODO: This must look up obj in dictionary
	inline EvaluatedArgument makeEvaluatedArgument(Varname const&)
	{
		return EvaluatedArgument{};
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

		Command& add(Literal&& arg) &;

		Command&& add(Literal&& arg) &&
		{
			return std::move(add(std::move(arg)));
		}

		Command& add(ExpandString&& str) &;

		Command&& add(ExpandString&& str) &&
		{
			return std::move(add(std::move(str)));
		}

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

	CommandOutput execute(Command const&, CommandOutput const& sysin);

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

	inline Pipe&& operator|(Pipe&& a, Command&& cmd)
	{
		return std::move(a |= std::move(cmd));
	}

	inline Pipe&& operator|(Pipe&& a, Literal&& x)
	{
		return std::move(a |= std::move(x));
	}

	CommandOutput execute(Pipe const& pipe, CommandOutput const& sysin);


	class CommandSplitOutput
	{
	public:
		CommandSplitOutput(): m_separator{'\0'}
		{
		}

		CommandSplitOutput& pipe(Pipe&& pipe) &
		{
			m_pipe = std::move(pipe);
			return *this;
		}


		CommandSplitOutput&& pipe(Pipe&& val) &&
		{
			return std::move(pipe(std::move(val)));
		}

		Pipe const& pipe() const
		{
			return m_pipe;
		}

		CommandSplitOutput& separator(char sep) &
		{
			m_separator = sep;
			return *this;
		}

		CommandSplitOutput&& separator(char sep) &&
		{
			return std::move(separator(sep));
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

		auto const& value() const
		{
			return m_value;
		}

		auto& value()
		{
			return m_value;
		}

		ExpandString& value(CommandSplitOutput&& command) &
		{
			m_value = std::move(command);
			return *this;
		}

		ExpandString&& value(CommandSplitOutput&& val) &&
		{
			return std::move(value(std::move(val)));
		}

		ExpandString& value(Varname&& name) &
		{
			m_value = std::move(name);
			return *this;
		}

		ExpandString&& value(Varname&& val) &&
		{
			return std::move(value(std::move(val)));
		}

		Literal const& suffix() const
		{
			return m_suffix;
		}

		ExpandString& suffix(Literal&& suffix) &
		{
			m_suffix = std::move(suffix);
			return *this;
		}

		ExpandString&& suffix(Literal&& val) &&
		{
			return std::move(suffix(std::move(val)));
		}

	private:
		Literal m_prefix;
		std::variant<Varname, CommandSplitOutput> m_value;
		Literal m_suffix;
	};

	inline bool operator==(ExpandString const& a, ExpandString const& b)
	{
		return a.prefix() == b.prefix() && a.value() == b.value() && a.suffix() == b.suffix();
	}

	inline bool operator!=(ExpandString const& a, ExpandString const& b)
	{
		return !(a == b);
	}

	inline Command& Command::add(Literal&& arg) &
	{
		m_args.push_back(std::move(arg));
		return *this;
	}

	inline Command& Command::add(ExpandString&& str) &
	{
		m_args.push_back(std::move(str));
		return *this;
	}

	EvaluatedArgument makeEvaluatedArgument(ExpandString const& obj);

	std::pair<Pipe, char const*> makePipe(char const* buffer);
}