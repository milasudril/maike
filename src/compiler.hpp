//@	{
//@	 "targets":[{"name":"compiler.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_COMPILER_HPP
#define MAIKE_COMPILER_HPP

#include "fs.hpp"
#include "compilation_log.hpp"
#include "key_value_store/compound.hpp"

#include <vector>
#include <memory>

namespace Maike
{
	class Compiler
	{
	public:
		Compiler(Compiler&&) = default;
		Compiler& operator=(Compiler&&) = default;

		Compiler(): m_handle{std::make_unique<NullCompiler>()} {};

		template<class Obj>
		explicit Compiler(Obj&& obj):
		   m_handle{std::make_unique<CompilerImpl<Obj>>(std::forward<Obj>(obj))}
		{
		}

		int run(fs::path const& src,
		        std::vector<fs::path const*> const& used_files,
		        std::vector<fs::path const*> const& output_files,
		        CompilationLog& log) const
		{
			return m_handle->run(src, used_files, output_files, log);
		}

		Maike::KeyValueStore::Compound settings() const
		{
			return m_handle->settings();
		}

		Compiler& settings(KeyValueStore::Compound&& cfg)
		{
			m_handle->settings(std::move(cfg));
			return *this;
		}

	private:
		class AbstractCompiler
		{
		public:
			virtual int run(fs::path const& src,
			                std::vector<fs::path const*> const& used_files,
			                std::vector<fs::path const*> const& output_files,
			                CompilationLog& log) const = 0;

			virtual Maike::KeyValueStore::Compound settings() const = 0;

			virtual void settings(KeyValueStore::Compound&& cfg) = 0;

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

			Maike::KeyValueStore::Compound settings() const override
			{
				return KeyValueStore::Compound{};
			}

			void settings(KeyValueStore::Compound&&) override
			{
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

			Maike::KeyValueStore::Compound settings() const override
			{
				return m_obj.settings();
			}

			void settings(KeyValueStore::Compound&& cfg) override
			{
				(void)m_obj.settings(std::move(cfg));
			}

			~CompilerImpl() override = default;

		private:
			T m_obj;
		};

		std::unique_ptr<AbstractCompiler> m_handle;
	};
}

#endif