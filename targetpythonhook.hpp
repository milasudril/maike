//@	{
//@	 "targets":[{"name":"targetpythonhook.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"targetpythonhook.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_TARGETPYTHONHOOK_HPP
#define MAIKE_TARGETPYTHONHOOK_HPP

#include "target_hook.hpp"

#include "targetpythoninterpreter.hpp"
#include "targetpythonloader.hpp"
#include "targetpythonfactory.hpp"

namespace Maike
	{
	class TargetPythonHook final:public Target_Hook
		{
		public:
			static TargetPythonHook* create(const ParameterSetDumpable& params);

			static TargetPythonHook* create(ParameterSet&& params)=delete;


			const TargetPythonFactory& factoryGet() const noexcept
				{return m_factory;}

			const TargetPythonLoader& loaderGet() const noexcept
				{return m_loader;}

			void configClear();

			TargetPythonHook& configAppendDefault();

			TargetPythonHook& configAppend(const ResourceObject& pythonoptions);

			void configDump(ResourceObject& pythonoptions) const;

		private:
			TargetPythonInterpreter m_intpret;
			TargetPythonLoader m_loader;
			TargetPythonFactory m_factory;

			explicit TargetPythonHook(const ParameterSetDumpable& params);

			void destroy() noexcept;
		};
	}

#endif
