//@	{
//@	 "targets":[{"name":"targetarchive.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetarchive.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETARCHIVESOURCE_HPP
#define MAIKE_TARGETARCHIVESOURCE_HPP

#include "../targetbase.hpp"

namespace Maike
	{
	class TargetArchiveCompiler;
	class PRIVATE TargetArchive final : public Maike::TargetBase
		{
		public:
			enum class Type:int{TAR,ZIP};

			static TargetArchive* create(const ResourceObject& obj
				,const TargetArchiveCompiler& compiler,const char* name_src
				,const char* in_dir,const char* root,size_t id,size_t line_count);

			static TargetArchive* create(const ResourceObject& obj
				,TargetArchiveCompiler&& compiler,const char* name_src
				,const char* in_dir,const char* root,size_t id
				,size_t line_count)=delete;

			bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const;

			void compileImpl(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir);

			void destroy() noexcept;

			void dumpDetails(ResourceObject& target) const;

			Type typeGet() const noexcept
				{return m_type;}

			const char* compressionGet() const noexcept
				{return m_compression.c_str();}

			const char* rootGet() const noexcept
				{return m_root.c_str();}

			bool generated() const noexcept
				{return 1;}

		private:
			const TargetArchiveCompiler& r_compiler;

			TargetArchive(const ResourceObject& obj
				,const TargetArchiveCompiler& intpret,const char* name_src
				,const char* in_dir,const char* root,size_t id,size_t line_count);

			TargetArchive(const ResourceObject& obj
				,TargetArchiveCompiler&& intpret,const char* name_src
				,const char* in_dir,const char* root,size_t id
				,size_t line_count)=delete;

			~TargetArchive() noexcept;
			Type m_type;
			std::string m_compression;
			std::string m_root;
		};
	}

#endif
