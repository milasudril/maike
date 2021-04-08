//@	{
//@	 "targets":[{"name":"invoker.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_INVOKER_HPP
#define MAIKE_INVOKER_HPP

#include "utils/fs.hpp"
#include "utils/varstring.hpp"

#include <vector>
#include <memory>

namespace Maike
{
	class ExitStatus
	{
	public:
		enum class Type
		{
			ReturnValue,
			Signal
		};

		constexpr explicit ExitSignal(Type type, int value): m_type{type}, m_value{value}
		{
		}

		constexpr int value() const
		{
			return m_value;
		}

		constexpr Type type() const
		{
			return m_type;
		}

	private:
		Type m_type;
		int m_value;
	};

	struct ExecResult
	{
		std::vector<std::byte> sysout;
		std::vector<std::byte> syserr;
		ExitStatus exit_status;
	};

	class Invoker
	{
	public:
		void mkdir(fs::path const& dir) const
		{
			return m_handle->mkdir(dir);
		}

		ExecResult execp(fs::path const& executable,
		                 std::vector<Varstring> const& args,
		                 std::vector<std::byte> const& sysin,
		                 char ifs) const
		{
			return m_handle->execp(executable, args, sysin, ifs);
		}

		void setvar(std::string&& name, std::string&& value)
		{
			m_handle->setvar(std::move(name), std::move(value));
		}

	private:
		class AbstractInvoker
		{
		public:
			virtual void mkdir(fs::path const& dir) const = 0;

			virtual ExecResult execp(fs::path const& executable,
			                         std::vector<Varstring> const& args,
			                         std::vector<std::byte> const& sysin,
			                         char ifs) const = 0;

			virtual void setvar(std::string&& name, std::string&& value) = 0;

			virtual ~AbstractInvoker()
			{
			}
		};

		template<class T>
		class InvokerImpl: public AbstractInvoker
		{
		public:
			explicit InvokerImpl(T&& obj): m_obj{std::move(obj)} {};

			void mkdir(fs::path const& dir) const override
			{
				return m_obj.mkdir(dir);
			}

			ExecResult execp(fs::path const& executable,
			                 std::vector<Varstring> const& args,
			                 std::vector<std::byte> const& sysin,
			                 char ifs) const override
			{
				return m_obj.execp(executable, args, sysin, ifs);
			}

			void setvar(std::string&& name, std::string&& value) override
			{
				m_obj.setvar(name, value);
			}

			~InvokerImpl() override = default;

		private:
			T m_obj;
		};

		std::unique_ptr<AbstractInvoker> m_handle;
	};
}

#endif