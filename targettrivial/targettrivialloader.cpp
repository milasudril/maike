//@	{"targets":[{"name":"targettrivialloader.o","type":"object"}]}

#include "targettrivialloader.hpp"
#include "targettrivial.hpp"
#include "../target_factorydelegator.hpp"
#include "../filein.hpp"
#include "../readbuffer.hpp"
#include "../dependencygraph.hpp"

#include <cstring>

using namespace Maike;

Handle<Target> TargetTrivialLoader::targetCreate(const ResourceObject&
	,const char*,const char*,const char*,size_t,size_t) const
	{return Handle<TargetTrivial>(nullptr);}

void TargetTrivialLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider&,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	auto suffix=strrchr(name_src,'.');
	auto targetname = suffix!=NULL? std::string{name_src, suffix}:std::string{name_src};
	targetname += r_opts.filenameExtGet();

	Handle<TargetTrivial> target{TargetTrivial::create(r_opts.commandGet(), targetname.c_str(), name_src,in_dir,factory.rootGet(),factory.idGet())};

	target->dependencyAdd(Dependency{r_opts.commandGet().nameGet(), Dependency::Relation::TOOL});

	graph.targetRegister(target);
	}
