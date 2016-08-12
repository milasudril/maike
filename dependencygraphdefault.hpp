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
	class PRIVATE DependencyGraphDefault:public DependencyGraph
		{
		public:
			explicit DependencyGraphDefault(EventHandler& handler):
				r_handler(handler),m_patch_needed(0)
				{}

			DependencyGraphDefault& targetRegister(Handle<Target>&& target);
			DependencyGraphDefault& targetsPatch();
			void targetsProcess(TargetProcessorConst&& proc) const;
			DependencyGraphDefault& targetsProcess(TargetProcessor&& proc);
			DependencyGraphDefault& targetsRemove(TargetProcessor&& condition);
			Target* targetFind(const Stringkey& key);
			const Target* targetFind(const Stringkey& key) const;

			const Twins<size_t>& idRangeGet() const;

			DependencyGraphDefault& clear() noexcept;

			size_t targetsCountGet() const noexcept
				{return m_targets.size();}

		private:
			EventHandler& r_handler;
			std::map< Stringkey,Handle<Target> > m_targets;
			Twins<size_t> m_id_range;
			bool m_patch_needed;
		};
	}

#endif
