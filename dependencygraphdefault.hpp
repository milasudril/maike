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
	class DependencyGraphDefault:public DependencyGraph
		{
		public:
			explicit DependencyGraphDefault(IdGenerator<size_t>& id_gen):
				r_id_gen(id_gen)
				{}

			DependencyGraphDefault& targetRegister(Handle<Target>&& target);
			DependencyGraphDefault& targetsPatch();
			DependencyGraphDefault& targetsProcess(TargetProcessor&& visitor);
			Target* targetFind(const Stringkey& key);

			const Twins<size_t>& idRangeGet() const noexcept
				{return m_id_range;}

		private:
			IdGenerator<size_t>& r_id_gen;
			std::map< Stringkey,Handle<Target> > m_targets;
			Twins<size_t> m_id_range;
		};
	}

#endif
