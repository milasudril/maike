//@	{
//@	 "targets":[{"name":"targetcp.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetcp.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETCP_HPP
#define MAIKE_TARGETCP_HPP

#include "../targetbase.hpp"
#include "../command.hpp"

namespace Maike
	{
	class PRIVATE TargetCP final : public Maike::TargetBase
		{
		public:
			static TargetCP* create(const ResourceObject& obj,const char* name_src
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
			TargetCP(const ResourceObject& obj,const char* name_src
				,const char* in_dir,const char* root,size_t id,size_t line_count);

			~TargetCP() noexcept;

			std::string m_src;
		};
	}

#endif
