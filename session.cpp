//@	{"targets":[{"name":"session.o","type":"object"}]}

#include "session.hpp"
#include "resourceobject.hpp"
#include "targetdirectory.hpp"
#include "exceptionhandler.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include "pathutils.hpp"
#include "targetconfig.hpp"
#include "resourceobjectjansson.hpp"
#include <cstring>

using namespace Maike;

Session::Session():
	 m_target_hooks(m_targetinfo)
	,m_evaluator(m_targetinfo)
	,m_spider(m_delegator,m_graph)
	,m_delegator(m_evaluator)
	,m_graph(m_delegator),m_dirty_flags(0)
	{
	configClear();
	rootSet("");
	}

Session& Session::configClear()
	{
	m_targetinfo.clear();
	m_source_files.clear();
	m_dirloader.configClear();
	m_spider.loadersUnregister();
	m_delegator.factoriesUnregister();
	m_target_hooks.configClear();
	graphDirtySet();
	targetHooksDirtySet();
	return *this;
	}

Session& Session::configAppendDefault()
	{
	sourceFileAppend(".");
	m_targetinfo.configAppendDefault();
	m_target_dir_full=dircat(m_delegator.rootGet()
		,static_cast<const char*>(m_targetinfo.variableGet(Stringkey("target_directory"))));
	m_dirloader.configAppendDefault();
	m_dirloader.pathReject(static_cast<const char*>(m_targetinfo.variableGet(Stringkey("target_directory"))));
	m_target_hooks.configAppendDefault();
	graphDirtySet();
	targetHooksDirtySet();
	return *this;
	}

Session& Session::sysvarsLoad()
	{
	m_targetinfo.sysvarsLoad();
	return *this;
	}

Session& Session::configAppend(const ResourceObject& maikeconfig)
	{
	if(maikeconfig.objectExists("source_files"))
		{
		auto source_files=maikeconfig.objectGet("source_files");
		auto N_sources=source_files.objectCountGet();
		for(decltype(N_sources) k=0;k<N_sources;++k)
			{
			sourceFileAppend(static_cast<const char*>(source_files.objectGet(k)));
			}
		}

	if(maikeconfig.objectExists("targetinfo"))
		{
		m_targetinfo.configAppend(maikeconfig.objectGet("targetinfo"));
		m_target_dir_full=dircat(m_delegator.rootGet()
			,static_cast<const char*>(m_targetinfo.variableGet(Stringkey("target_directory"))));
		}

	if(maikeconfig.objectExists("directoryoptions"))
		{
		m_dirloader.configAppend(maikeconfig.objectGet("directoryoptions"))
			.pathReject(static_cast<const char*>(m_targetinfo.variableGet(Stringkey("target_directory"))));
		}

	if(maikeconfig.objectExists("target_hooks"))
		{
		m_target_hooks.configAppend(maikeconfig.objectGet("target_hooks"));
		}

	graphDirtySet();
	targetHooksDirtySet();
	return *this;
	}

Session& Session::hookRegister(const char* name_plugin,Twins<const char* const*> filename_exts)
	{
	m_target_hooks.hookRegister(name_plugin,filename_exts);
	return *this;
	}

void Session::configDump(ResourceObject& maikeconfig) const
	{
		{
		auto source_files=maikeconfig.createArray();
		auto i=m_source_files.begin();
		auto i_end=m_source_files.end();
		while(i!=i_end)
			{
			source_files.objectAppend(source_files.create(i->c_str()));
			++i;
			}
		maikeconfig.objectSet("source_files",std::move(source_files));
		}

		{
		auto targetinfo=maikeconfig.createObject();
		m_targetinfo.configDump(targetinfo);
		maikeconfig.objectSet("targetinfo",std::move(targetinfo));
		}

		{
		auto directoryoptions=maikeconfig.createObject();
		m_dirloader.configDump(directoryoptions);
		maikeconfig.objectSet("directoryoptions",std::move(directoryoptions));
		}

		{
		auto target_hooks=maikeconfig.createArray();
		m_target_hooks.configDump(target_hooks);
		maikeconfig.objectSet("target_hooks",std::move(target_hooks));
		}
	}

Session& Session::rootSet(const char* root)
	{
	m_delegator.rootSet(root);
	m_targetinfo.variableSet("project_root",root);
	m_target_dir_full=dircat(root
		,static_cast<const char*>(m_targetinfo.variableGet(Stringkey("target_directory"))));
	graphDirtySet();
	return *this;
	}

const char* Session::rootGet()	const noexcept
	{
	return m_delegator.rootGet();
	}

Session& Session::scanRecursiveSet(bool recursive) noexcept
	{
	m_dirloader.recursiveSet(recursive);
	return *this;
	}

bool Session::scanRecursiveGet() const noexcept
	{
	return m_dirloader.recursiveGet();
	}

Session& Session::sourceFileAppend(const char* filename)
	{
	m_source_files.insert(filename);
	graphDirtySet();
	return *this;
	}



namespace
	{
	class BySourceName:public DependencyGraph::TargetProcessor
		{
		public:
			BySourceName(const char* filename):r_filename(filename){}

			int operator()(DependencyGraph& graph,Target& target)
				{return strcmp(target.sourceNameGet(),r_filename);}

		private:
			const char* r_filename;
		};

	class ByParentDirectory:public DependencyGraph::TargetProcessor
		{
		public:
			ByParentDirectory(const char* filename):r_filename(filename){}

			int operator()(DependencyGraph& graph,Target& target)
				{return strcmp(target.inDirGet(),r_filename);}

		private:
			const char* r_filename;
		};
	}

Session& Session::scanFile(const char* filename)
	{
	auto name_full=dircat(rootGet(),filename);
		{
		auto t=m_graph.targetFind(Stringkey(name_full.c_str()));
		if(dynamic_cast<TargetDirectory*>(t))
			{m_graph.targetsRemove(ByParentDirectory(t->sourceNameGet()));}
		}
	m_graph.targetsRemove(BySourceName(name_full.c_str()));

	if(targetHooksDirty())
		{targetHooksRegister();}

	m_spider.scanFile(name_full.c_str(),dirname(name_full.c_str()).c_str()).run();
	graphDirtyClear();
	return *this;
	}

void Session::dependenciesClear() noexcept
	{
	m_graph.clear();
	graphDirtySet();
	}

void Session::dependenciesReload() const
	{
	m_graph.clear();
	
		{
		ResourceObjectJansson config(ResourceObject::Type::OBJECT);
		configDump(config);
		m_graph.targetRegister(Handle<Target>(TargetConfig::create(config,m_delegator.idGet())));
		}

	auto i=m_source_files.begin();
	auto i_end=m_source_files.end();
	while(i!=i_end)
		{
		auto name_full=dircat(rootGet(),i->c_str());
		m_spider.scanFile(name_full.c_str(),dirname(name_full.c_str()).c_str());
		++i;
		}
	if(targetHooksDirty())
		{targetHooksRegister();}
	m_spider.run();
	graphDirtyClear();
	}


namespace
	{
	class TargetHookRegistrator:public Target_Hook_Registry::EnumCallbackFilenameExt
		{
		public:
			explicit TargetHookRegistrator(Spider& spider,Target_FactoryDelegator& delegator) noexcept:
				r_spider(spider),r_delegator(delegator)
				{}

			void operator()(const Stringkey& filename_ext,const Target_Hook& hook)
				{
				r_spider.loaderRegister(filename_ext,hook.loaderGet());
				r_delegator.factoryRegister(filename_ext,hook.factoryGet());
				}
		private:
			Spider& r_spider;
			Target_FactoryDelegator& r_delegator;
		};
	}

void Session::targetHooksRegister() const
	{
	m_target_hooks.enumerate(TargetHookRegistrator(m_spider,m_delegator));
	m_spider.loaderRegister(Stringkey("."),m_dirloader);
	targetHooksDirtyClear();
	}

void Session::targetsProcess(DependencyGraph::TargetProcessor&& proc)
	{
	if(graphDirty())
		{dependenciesReload();}

	m_graph.targetsProcess(std::move(proc));
	}

void Session::targetsProcess(DependencyGraph::TargetProcessorConst&& proc) const
	{
	if(graphDirty())
		{dependenciesReload();}

	m_graph.targetsProcess(std::move(proc));
	}

const Target& Session::target(const char* name) const
	{
	if(graphDirty())
		{dependenciesReload();}
	auto ret=m_graph.targetFind(Stringkey(name));
	if(ret==nullptr)
		{exceptionRaise(ErrorMessage("Target #0; has not been loaded.",{name}));}
	return *ret;
	}

Target& Session::target(const char* name)
	{
	if(graphDirty())
		{dependenciesReload();}
	auto ret=m_graph.targetFind(Stringkey(name));
	if(ret==nullptr)
		{exceptionRaise(ErrorMessage("Target #0; has not been loaded.",{name}));}
	return *ret;
	}

const char* Session::targetDirectoryGet() const noexcept
	{
	return m_target_dir_full.c_str();
	}

const Twins<size_t>& Session::targetIdRangeGet() const
	{
	if(graphDirty())
		{dependenciesReload();}
	return m_graph.idRangeGet();
	}

size_t Session::targetsCountGet() const
	{
	if(graphDirty())
		{dependenciesReload();}
	return m_graph.targetsCountGet();
	}

bool Session::loaderHas(const char* filename) const
	{
	auto filename_ext=strrchr(filename,'.');
	if(filename_ext==NULL)
		{return 0;}
	if(targetHooksDirty())
		{targetHooksRegister();}
	return m_spider.loaderHas(Stringkey(filename_ext));
	}
