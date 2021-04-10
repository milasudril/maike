//@	{
//@	 "targets":[{"name":"invoker.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_INVOKER_HPP
#define MAIKE_INVOKER_HPP

#include "src/utils/fs.hpp"

#include <vector>
#include <memory>

namespace Maike::CommandInterpreter
{
	class Invoker
	{
	public:
		using ExecResult = std::vector<std::byte>;

		template<class T>
		explicit Invoker(std::reference_wrapper<T> obj):
		   r_handle{&obj.get()},
		   m_vt{[](void const* handle,
		           fs::path const& executable,
		           std::vector<std::string> const& args,
		           std::vector<std::byte> const& sysin) {
			        return static_cast<T const*>(handle)->execp(executable, args, sysin);
		        },
		        [](void const* handle, std::string_view name) -> std::vector<std::string> const& {
			        return static_cast<T const*>(handle)->getvar(name);
		        }}
		{
		}

		ExecResult execp(fs::path const& executable,
		                 std::vector<std::string> const& args,
		                 std::vector<std::byte> const& sysin) const
		{
			return m_vt.execp(r_handle, executable, args, sysin);
		}

		std::vector<std::string> const& getvar(std::string_view name) const
		{
			return m_vt.getvar(r_handle, name);
		}

	private:
		void* r_handle;
		struct Vtable
		{
			ExecResult (*execp)(void const*,
			                    fs::path const&,
			                    std::vector<std::string> const&,
			                    std::vector<std::byte> const&);
			std::vector<std::string> const& (*getvar)(void const*, std::string_view);
		};
		Vtable m_vt;
	};
}

#endif