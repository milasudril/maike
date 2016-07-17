//@	{
//@	 "targets":[{"name":"targetpython.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetpython.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETPYTHONSOURCE_HPP
#define MAIKE_TARGETPYTHONSOURCE_HPP

#include "targetbase.hpp"

namespace Maike
	{
	class TargetPythonInterpreter;
	class TargetPython final : public Maike::TargetBase
		{
		public:
			static TargetPython* create(const ResourceObject& obj
				,const TargetPythonInterpreter& compiler,const char* name_src
				,const char* in_dir,size_t id,size_t line_count);

			static TargetPython* create(const ResourceObject& obj
				,TargetPythonInterpreter&& compiler,const char* name_src
				,const char* in_dir,size_t id,size_t line_count)=delete;

			bool upToDate(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir) const
				{return m_status==0? 1 : 0;}

			void compileImpl(Twins<const Dependency*> dependency_list
				,Twins<const Dependency*> dependency_list_full
				,const char* target_dir);

			void destroy() noexcept;

			void dumpDetails(ResourceObject& target) const;

		private:
			const TargetPythonInterpreter& r_intpret;

			TargetPython(const ResourceObject& obj
				,const TargetPythonInterpreter& intpret,const char* name_src
				,const char* in_dir,size_t id,size_t line_count);

			TargetPython(const ResourceObject& obj
				,TargetPythonInterpreter&& intpret,const char* name_src
				,const char* in_dir,size_t id,size_t line_count)=delete;

			~TargetPython();

			std::vector<std::string> m_args;
			int m_status;
		};
	}

#endif
