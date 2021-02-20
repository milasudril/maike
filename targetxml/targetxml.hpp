//@	{
//@	 "targets":[{"name":"targetxml.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetxml.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETXML_HPP
#define MAIKE_TARGETXML_HPP

#include "../targetbase.hpp"
#include "../command.hpp"

namespace Maike
	{
	class PRIVATE TargetXML final : public Maike::TargetBase
		{
		public:
			static TargetXML* create(const ResourceObject& obj,const char* name_src
				,const char* in_dir,const char* root,size_t id,size_t line_count);

			bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const override;

			void compileImpl(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) override;

			void destroy() noexcept override;

			void dumpDetails(ResourceObject& target) const override;

			bool generated() const noexcept override
				{return 1;}

		private:
			TargetXML(const ResourceObject& obj,const char* name_src
				,const char* in_dir,const char* root,size_t id,size_t line_count);

			~TargetXML() noexcept;

			Command m_cmd;
			std::string m_root;
		};
	}

#endif
