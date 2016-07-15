//@	{"targets":[{"name":"target_hook.hpp","type":"include"}]}

#ifndef MAIKE_TARGET_HOOK_HPP
#define MAIKE_TARGET_HOOK_HPP

namespace Maike
	{
	class Target_Factory;
	class Target_Loader;
	class ResourceObject;
	class ParameterSet;

	class Target_Hook
		{
		public:
			typedef Target_Hook Base;
			static void destroy(Target_Hook* self) noexcept
				{self->destroy();}

			virtual const Target_Factory& factoryGet() const noexcept=0;
			virtual const Target_Loader& loaderGet() const noexcept=0;
			virtual void configClear()=0;
			virtual Target_Hook& configAppendDefault()=0;
			virtual Target_Hook& configAppend(const ResourceObject& config)=0;
			virtual void configDump(ResourceObject& config) const=0;

		protected:
			~Target_Hook()=default;
		private:
			virtual void destroy() noexcept=0;
		};
	}

extern "C"
	{
	Maike::Target_Hook* Maike_Target_Hook_create(const Maike::ParameterSet& params);
	}

#endif
