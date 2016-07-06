//@	{
//@	 "targets":[{"name":"targetcxx.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcxx.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCXX_HPP
#define MAIKE_TARGETCXX_HPP

#include "targetbase.hpp"
#include "targetcxxoptions.hpp"

namespace Maike
	{
	class TargetCxxCompiler;
	class TargetCxx final:public TargetBase
		{
		public:
			enum class Type:unsigned int
				{INCLUDE,OBJECT,APPLICATION,LIB_DYNAMIC,LIB_STATIC};

			static TargetCxx* create(const ResourceObject& obj
				,const TargetCxxCompiler& compiler
				,const char* name_src
				,const char* in_dir,size_t id);

			static TargetCxx* create(const ResourceObject& obj
				,TargetCxxCompiler&& compiler
				,const char* name_src
				,const char* in_dir,size_t id)=delete;

			void compile(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir);

			bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const;

			void destroy() noexcept;

			Type typeGet() const noexcept
				{return m_type;}

		private:
			TargetCxx(const ResourceObject& obj
				,const TargetCxxCompiler& compiler
				,const char* name_src
				,const char* in_dir,size_t id);

			TargetCxx(const ResourceObject& obj
				,TargetCxxCompiler&& compiler
				,const char* name_src
				,const char* in_dir,size_t id)=delete;

			Type m_type;
			const TargetCxxCompiler& r_compiler;
			TargetCxxOptions m_options_extra;
		};
	}

#endif