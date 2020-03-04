//@	{
//@	 "targets":[{"name":"source_file_info.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"source_file_info.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_SOURCEFILEINFO_HPP
#define MAIKE_SOURCEFILEINFO_HPP

#include "./basic_dependency.hpp"
#include "./compiler.hpp"
#include "./fs.hpp"
#include <vector>

namespace Maike
{
	class SourceFileInfo;

	using Dependency = BasicDependency<SourceFileInfo>;

	class SourceFileInfo
	{
	public:
		/**
		 * Allows to create a node without a source file to be scanned.
		 */
		explicit SourceFileInfo() = default;

		explicit SourceFileInfo(std::vector<Dependency>&& used_files,
		                        fs::path const& target_dir,
		                        std::vector<fs::path>&& targets,
		                        Compiler&& compiler);

		std::vector<Dependency> const& usedFiles() const
		{
			return m_used_files;
		}

		std::vector<fs::path> const& targets() const
		{
			return m_targets;
		}

		Compiler const& compiler() const
		{
			return m_compiler;
		}

	private:
		std::vector<Dependency> m_used_files;
		std::vector<fs::path> m_targets;
		Compiler m_compiler;
	};

	class ConstTag{};
	class NonConstTag{};

	template<class Tag>
	class SourceFile
	{
	public:
		static_assert(std::is_same_v<Tag, ConstTag> || std::is_same_v<Tag, NonConstTag>);

		static constexpr auto IsConst = std::is_same_v<Tag, ConstTag>;

		using WrappedSourceFileInfo = std::conditional_t<IsConst, std::add_const_t<SourceFileInfo>, SourceFileInfo>;

		SourceFile():r_name{nullptr}, r_info{nullptr} {}

		explicit SourceFile(fs::path const&& name, WrappedSourceFileInfo& info) = delete;

		explicit SourceFile(fs::path const&& name, SourceFileInfo const&& info) = delete;

		explicit SourceFile(fs::path const& name, WrappedSourceFileInfo& info): r_name{&name}, r_info{&info}
		{
		}

		bool valid() const
		{ return r_name != nullptr; }

		fs::path const& name() const
		{
			return *r_name;
		}

		bool targetsUpToDate() const;

		int compile();

		std::vector<Dependency> const& usedFiles() const
		{
			return r_info->usedFiles();
		}

		std::vector<fs::path> const& targets() const
		{
			return r_info->targets();
		}

		Compiler const& compiler() const
		{
			return r_info->compiler();
		}

		operator SourceFile<ConstTag>() const
		{
			return SourceFile<ConstTag>{*r_name, *r_info};
		}

	private:
		fs::path const* r_name;
		WrappedSourceFileInfo* r_info;
	};


	SourceFile(fs::path const& name, SourceFileInfo& info) -> SourceFile<NonConstTag>;

	SourceFile(fs::path const& name, SourceFileInfo const& info) -> SourceFile<ConstTag>;
}

#endif