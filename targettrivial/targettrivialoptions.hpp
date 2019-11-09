//@	{
//@	 "targets":[{"name":"targettrivialoptions.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targettrivialoptions.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETTRIVIALOPTIONS_HPP
#define MAIKE_TARGETTRIVIALOPTIONS_HPP

#include "../command.hpp"
#include "../dependency.hpp"
#include <string>

namespace Maike
	{
	class ResourceObject;

	class PRIVATE TargetTrivialOptions
		{
		public:
			TargetTrivialOptions();

			explicit TargetTrivialOptions(const ResourceObject& options);

			const Command& commandGet() const noexcept
				{return m_build_cmd;}

			const char* filenameExtGet() const noexcept
				{return m_filename_ext.c_str();}

			Twins<const Dependency*> dependenciesGet() const noexcept
				{
				return
					{
					 m_deps.data()
					,m_deps.data() + m_deps.size()
					};
				}

			void configClear();
			TargetTrivialOptions& configAppendDefault();
			TargetTrivialOptions& configAppend(const ResourceObject& options);
			TargetTrivialOptions& configAppend(const TargetTrivialOptions& options);

			void configDump(ResourceObject& options) const;

		private:
			std::string m_filename_ext;
			std::vector<Dependency> m_deps;
			Command m_build_cmd;
		};
	}

#endif
