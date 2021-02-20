//@	{
//@	 "targets":[{"name":"dependencygraphdefault.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"dependencygraphdefault.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_DEPENDENCYGRAPHDEFAULT_HPP
#define MAIKE_DEPENDENCYGRAPHDEFAULT_HPP

#include "dependencygraph.hpp"
#include "stringkey.hpp"
#include "handle.hpp"
#include "twins.hpp"
#include "idgenerator.hpp"
#include <map>

namespace Maike
	{
	class PRIVATE DependencyGraphDefault final:public DependencyGraph
		{
		public:
			explicit DependencyGraphDefault(EventHandler& handler):
				r_handler(handler),m_patch_needed(0)
				{}

			DependencyGraphDefault& targetRegister(Handle<Target>&& target) override;
			DependencyGraphDefault& targetsPatch() override;
			void targetsProcess(TargetProcessorConst&& proc) const override;
			DependencyGraphDefault& targetsProcess(TargetProcessor&& proc) override;
			DependencyGraphDefault& targetsRemove(TargetProcessor&& condition) override;
			Target* targetFind(const Stringkey& key) override;
			const Target* targetFind(const Stringkey& key) const override;

			const Twins<size_t>& idRangeGet() const override;

			DependencyGraphDefault& clear() noexcept override;

			size_t targetsCountGet() const noexcept override
				{return m_targets.size();}

		private:
			EventHandler& r_handler;
			std::map< Stringkey,Handle<Target> > m_targets;
			Twins<size_t> m_id_range;
			bool m_patch_needed;
		};
	}

#endif
