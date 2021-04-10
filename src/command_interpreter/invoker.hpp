//@	{
//@	 "targets":[{"name":"invoker.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_INVOKER_HPP
#define MAIKE_INVOKER_HPP

#include "src/utils/fs.hpp"

#include <vector>
#include <memory>

namespace Maike
{
	class Invoker
	{
	public:
		using ExecResult = std::vector<std::byte>;

		ExecResult execp(fs::path const& executable,
		                 std::vector<std::string> const& args,
		                 std::vector<std::byte> const& sysin) const
		{
			return m_handle->execp(executable, args, sysin);
		}

		std::vector<std::string> const& getvar(std::string_view name) const
		{
			return m_handle->getvar(name);
		}

	private:
		class AbstractInvoker
		{
		public:
			virtual ExecResult execp(fs::path const& executable,
			                         std::vector<std::string> const& args,
			                         std::vector<std::byte> const& sysin) const = 0;

			virtual std::vector<std::string> const& getvar(std::string_view name) = 0;

			virtual ~AbstractInvoker()
			{
			}
		};

		template<class T>
		class InvokerImpl: public AbstractInvoker
		{
		public:
			explicit InvokerImpl(T&& obj): m_obj{std::move(obj)} {};

			ExecResult execp(fs::path const& executable,
			                 std::vector<std::string> const& args,
			                 std::vector<std::byte> const& sysin) const override
			{
				return m_obj.execp(executable, args, sysin);
			}

			std::vector<std::string> const& getvar(std::string_view name) override
			{
				return m_obj.getvar(name);
			}

			~InvokerImpl() override = default;

		private:
			T m_obj;
		};

		std::unique_ptr<AbstractInvoker> m_handle;
	};
}

#endif