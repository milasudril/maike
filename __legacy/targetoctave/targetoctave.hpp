//@	{
//@	 "targets":[{"name":"targetoctave.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetoctave.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETOCTAVESOURCE_HPP
#define MAIKE_TARGETOCTAVESOURCE_HPP

#include "../targetbase.hpp"

namespace Maike
{
	class TargetOctaveInterpreter;
	class PRIVATE TargetOctave final: public Maike::TargetBase
	{
	public:
		static TargetOctave* create(const ResourceObject& obj,
		                            const TargetOctaveInterpreter& compiler,
		                            const char* name_src,
		                            const char* in_dir,
		                            const char* root,
		                            size_t id,
		                            size_t line_count);

		static TargetOctave* create(const ResourceObject& obj,
		                            TargetOctaveInterpreter&& compiler,
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
		const TargetOctaveInterpreter& r_intpret;

		TargetOctave(const ResourceObject& obj,
		             const TargetOctaveInterpreter& intpret,
		             const char* name_src,
		             const char* in_dir,
		             const char* root,
		             size_t id,
		             size_t line_count);

		TargetOctave(const ResourceObject& obj,
		             TargetOctaveInterpreter&& intpret,
		             const char* name_src,
		             const char* in_dir,
		             const char* root,
		             size_t id,
		             size_t line_count) = delete;

		~TargetOctave() noexcept;

		std::vector<std::string> m_args;
		std::string m_root;
		int m_status;
		bool m_static;
	};
}

#endif
