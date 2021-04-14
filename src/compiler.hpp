//@	{
//@	 "targets":[{"name":"compiler.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_COMPILER_HPP
#define MAIKE_COMPILER_HPP

#include "./compilation_log.hpp"
#include "src/key_value_store/compound.hpp"
#include "src/utils/fs.hpp"

#include <vector>
#include <memory>
#include <type_traits>

namespace Maike
{
	class Compiler
	{
	public:
		Compiler(Compiler&&) = default;
		Compiler& operator=(Compiler&&) = default;

		Compiler(): m_handle{std::make_unique<NullCompiler>()} {};

		template<class Obj, std::enable_if_t<!std::is_same_v<Obj, Compiler>, int> = 0>
		explicit Compiler(Obj&& obj):
		   m_recipe{std::remove_reference_t<Obj>::defaultRecipe()},
		   m_handle{std::make_unique<CompilerImpl<Obj>>(std::forward<Obj>(obj))}
		{
		}

		fs::path const& recipe() const
		{
			return m_recipe;
		}

	private:
		class AbstractCompiler
		{
		public:
			virtual int run(fs::path const& src,
			                std::vector<fs::path const*> const& used_files,
			                std::vector<fs::path const*> const& output_files,
			                CompilationLog& log) const = 0;

			virtual ~AbstractCompiler()
			{
			}
		};

		class NullCompiler: public AbstractCompiler
		{
		public:
			int run(fs::path const&,
			        std::vector<fs::path const*> const&,
			        std::vector<fs::path const*> const&,
			        CompilationLog&) const override
			{
				return 0;
			}

			~NullCompiler() override = default;

		private:
		};

		template<class T>
		class CompilerImpl: public AbstractCompiler
		{
		public:
			explicit CompilerImpl(T&& obj): m_obj{std::move(obj)} {};

			int run(fs::path const& src,
			        std::vector<fs::path const*> const& used_files,
			        std::vector<fs::path const*> const& output_files,
			        CompilationLog& log) const override
			{
				return m_obj.run(src, used_files, output_files, log);
			}

			~CompilerImpl() override = default;

		private:
			T m_obj;
		};

		fs::path m_recipe;
		std::unique_ptr<AbstractCompiler> m_handle;
	};
}

#endif