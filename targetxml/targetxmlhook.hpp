//@	{
//@	 "targets":[{"name":"targetxmlhook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetxmlhook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETXMLHOOK_HPP
#define MAIKE_TARGETXMLHOOK_HPP

#include "../target_hook.hpp"

#include "targetxmlloader.hpp"
#include "targetxmlfactory.hpp"

namespace Maike
	{
	class PRIVATE TargetXMLHook final:public Target_Hook
		{
		public:
			static TargetXMLHook* create(const ParameterSetDumpable& params);

			static TargetXMLHook* create(ParameterSet&& params)=delete;


			const TargetXMLFactory& factoryGet() const noexcept
				{return m_factory;}

			const TargetXMLLoader& loaderGet() const noexcept
				{return m_loader;}

			void configClear();

			TargetXMLHook& configAppendDefault();

			TargetXMLHook& configAppend(const ResourceObject& pythonoptions);

			void configDump(ResourceObject& pythonoptions) const;

		private:
			TargetXMLLoader m_loader;
			TargetXMLFactory m_factory;

			explicit TargetXMLHook(const ParameterSetDumpable& params);

			void destroy() noexcept;
		};
	}

#endif
