//@	{
//@	 "targets":[{"name":"invoker.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_INVOKER_HPP
#define MAIKE_INVOKER_HPP

#include "./compilation_log.hpp"
#include "src/key_value_store/compound.hpp"
#include "utils/fs.hpp"

#include <vector>
#include <memory>
#include <variant>

namespace Maike
{
	class ExitSignal
	{
	public:
		constexpr explicit ExitSignal(int value): m_value{value} {}

		constexpr int value() const
		{ return m_value; }

	private:
		int m_value;
	};

	class ExitStatus
	{
	public:
		constexpr explicit ExitSignal(int value): m_value{value} {}

		constexpr int value() const
		{ return m_value; }

	private:
		int m_value;
	};

	struct ExecResult
	{
		std::vector<std::byte> sysout;
		std::vector<std::byte> syserr;
		std::variant<ExitStatus, ExitSignal> exit_result;
	};

	class Invoker
	{
	public:
		void mkdir(fs::path const& dir) const
		{ return m_handle->mkdir(fs::path const& dir); }

		ExecResult execp(fs::path const& executable,
		                     std::vector<std::string> const& args,
		                     std::vector<std::byte> const& sysin) const
		{
			return m_handle->execp(executable, args, sysin);
		}

	private:
		class AbstractInvoker
		{
		public:
			virtual void mkdir(fs::path const& dir) const = 0;

			virtual ExecResult execp(fs::path const& executable, std::vector<std::string> const& args, std::vector<std::byte> const& sysin) const = 0;

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
			{ return m_handle->mkdir(fs::path const& dir); }

			ExecResult execp(fs::path const& executable,
									std::vector<std::string> const& args,
									std::vector<std::byte> const& sysin) const override
			{
				return m_handle->execp(executable, args, sysin);
			}

			~InvokerImpl() override = default;

		private:
			T m_obj;
		};

		std::unique_ptr<AbstractInvoker> m_handle;
	};
}

#endif