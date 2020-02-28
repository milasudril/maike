//@	{
//@	 "targets":[{"name":"compiler.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_COMPILER_HPP
#define MAIKE_COMPILER_HPP

#include "path.hpp"
#include "configuration.hpp"
#include "compilation_log.hpp"

#include <vector>
#include <memory>

namespace Maike
{
	class Compiler
	{
		public:
			template<class Obj>
			explicit Compiler(Obj&& obj):m_handle{std::make_unique<CompilerImpl<Obj>>{std::forward<Obj>(obj)}}
			{
			}

			int run(Path const& src,
                     std::vector<Path const*> const& used_files,
                     std::vector<Path const*> const& output_files,
                     CompilationLog& log) const
			{
				m_handle->run(src, used_files, output_files, log);
			}

            DataStore settings() const
            {
				return m_handle->settings();
            }

            Compiler& settings(DataStore const& cfg)
            {
				m_handle->settings(cfg);
				return *this;
            }

		private:
			class AbstractCompiler
			{
				public:
					virtual int run(Path const& src,
                                    std::vector<Path const*> const& used_files,
                                    std::vector<Path const*> const& output_files,
                                    CompilationLog& log) const = 0;

					virtual DataStore settings() const = 0;

					virtual void settings(DataStore const& cfg) = 0;

					virtual ~AbstractCompiler() = 0;
			};

			template<class T>
			class CompilerImpl:public AbstractCompiler
			{
				public:
					explicit CompilerImpl(T&& obj):m_obj{std::move(obj)}{};

					int run(Path const& src,
                                    std::vector<Path const*> const& used_files,
                                    std::vector<Path const*> const& output_files,
                                    CompilationLog& log) const override
					{ return m_obj.run(src, used_files, output_files, log); }

					DataStore settings() const
					{ return m_obj.settings(); }

					void settings(DataStore const& cfg)
					{ (void*)m_obj.settings(cfg); }

					~AbstractCompiler() override = default;

				private:
					T m_obj;
			};

			std::unique_ptr<AbstractCompiler> m_handle;
	};
}

#endif