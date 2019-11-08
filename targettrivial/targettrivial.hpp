//@	{
//@	 "targets":[{"name":"targettrivial.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targettrivial.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETTRIVIAL_HPP
#define MAIKE_TARGETTRIVIAL_HPP

#include "../targetbase.hpp"
#include "../command.hpp"

namespace Maike
	{
	class PRIVATE TargetTrivial final : public Maike::TargetBase
		{
		public:
			static TargetTrivial* create(const ResourceObject& obj,const char* name_src
				,const char* in_dir,const char* root,size_t id,size_t line_count);

			bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const;

			void compileImpl(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir);

			void destroy() noexcept;

			void dumpDetails(ResourceObject& target) const;

			bool generated() const noexcept
				{return 1;}

		private:
			TargetTrivial(const ResourceObject& obj,const char* name_src
				,const char* in_dir,const char* root,size_t id,size_t line_count);

			~TargetTrivial() noexcept;

			std::string m_src;
		};
	}

#endif
