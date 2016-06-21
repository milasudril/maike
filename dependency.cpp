//@ {
//@ "targets":[{"name":"dependency.o","type":"object"}]
//@ }

#include "dependency.hpp"

using namespace Maike;

Dependency::Dependency(const char* name,Relation relation):
	m_name(name),r_target(nullptr),m_rel(relation)
	{}

Dependency::Dependency():r_target(nullptr),m_rel(Relation::LEAF)
	{}
