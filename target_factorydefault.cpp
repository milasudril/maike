//@	{"targets":[{"name":"target_factorydefault.o","type":"object"}]}

#include "target_factorydefault.hpp"
#include "resourceobject.hpp"

using namespace Maike;

Target_FactoryDefault::Target_FactoryDefault(const ResourceObject& config
	,const ParameterSet& sysvars):
	m_cxxoptions(config.objectGet("cxxoptions")),m_cxxcompiler(m_cxxoptions,sysvars)
	,m_pyintpret(config.objectGet("pythonoptions"))
	{
	}
