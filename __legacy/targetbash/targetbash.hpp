//@	{
//@	 "targets":[{"name":"targetbash.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetbash.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETBASH_HPP
#define MAIKE_TARGETBASH_HPP

#include "../targetbase.hpp"

namespace Maike
{
	class TargetBashInterpreter;

	class PRIVATE TargetBash final: public Maike::TargetBase
	{
	public:
		static TargetBash* create(const ResourceObject& obj,
		                          const TargetBashInterpreter& compiler,
		                          const char* name_src,
		                          const char* in_dir,
		                          const char* root,
		                          size_t id,
		                          size_t line_count);

		static TargetBash* create(const ResourceObject& obj,
		                          TargetBashInterpreter&& compiler,
		                          const char* name_src,
		                          const char* in_dir,
		                          const char* root,
		                          size_t id,
		                          size_t line_count) = delete;

		bool upToDate(Twins<const Dependency*> dependency_list,
		              Twins<const Dependency*> dependency_list_full,
		              const char* target_dir) const;

		void compileImpl(Twins<const Dependency*> dependency_list,
		                 Twins<const Dependency*> dependency_list_full,
		                 const char* target_dir);

		void destroy() noexcept;

		void dumpDetails(ResourceObject& target) const;

		bool generated() const noexcept
		{
			return 1;
		}

	private:
		const TargetBashInterpreter& r_intpret;

		TargetBash(const ResourceObject& obj,
		           const TargetBashInterpreter& intpret,
		           const char* name_src,
		           const char* in_dir,
		           const char* root,
		           size_t id,
		           size_t line_count);

		TargetBash(const ResourceObject& obj,
		           TargetBashInterpreter&& intpret,
		           const char* name_src,
		           const char* in_dir,
		           const char* root,
		           size_t id,
		           size_t line_count) = delete;

		~TargetBash() noexcept;

		std::vector<std::string> m_args;
		std::string m_root;
		int m_status;
		bool m_static;
	};
}

#endif
