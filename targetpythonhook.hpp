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
			static TargetPythonHook* create(const ResourceObject& pythonoptions
				,const ParameterSet& params);


			const TargetPythonFactory& factoryGet() const noexcept
				{return m_factory;}

			const TargetPythonLoader& loaderGet() const noexcept
				{return m_loader;}

			void configClear();

			TargetPythonHook& configAppend(const ResourceObject& pythonoptions);

		private:
			TargetPythonInterpreter m_intpret;
			TargetPythonLoader m_loader;
			TargetPythonFactory m_factory;

			explicit TargetPythonHook(const ResourceObject& pythonoptions
				,const ParameterSet& params);

			explicit TargetPythonHook(const ResourceObject& pythonoptions
				,ParameterSet&& params)=delete;

			void destroy() noexcept;
		};
	}

#endif
