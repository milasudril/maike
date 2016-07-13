//@	{"targets":[{"name":"session.o","type":"object"}]}

#include "session.hpp"
#include "resourceobject.hpp"
#include "targetdirectory.hpp"
#include "exceptionhandler.hpp"
#include "errormessage.hpp"
#include "variant.hpp"
#include <cstring>

using namespace Maike;

Session::Session():
	 m_cxxhook(TargetCxxHook::create(m_targetinfo))
	,m_pythonhook(TargetPythonHook::create(m_targetinfo))
	,m_evaluator(m_targetinfo)
	,m_delegator(static_cast<const char*>(m_targetinfo.variableGet(Stringkey("target_directory")))
		,m_evaluator,m_id_gen)
	,m_graph(m_id_gen)
	,m_spider(m_delegator,m_graph)
	{
	m_delegator.factoryRegister(Stringkey(".cpp"),m_cxxhook->factoryGet())
		.factoryRegister(Stringkey(".hpp"),m_cxxhook->factoryGet())
		.factoryRegister(Stringkey(".h"),m_cxxhook->factoryGet())
		.factoryRegister(Stringkey(".py"),m_pythonhook->factoryGet());

	m_spider.loaderRegister(Stringkey("."),m_dirloader)
		.loaderRegister(Stringkey(".cpp"),m_cxxhook->loaderGet())
		.loaderRegister(Stringkey(".hpp"),m_cxxhook->loaderGet())
		.loaderRegister(Stringkey(".h"),m_cxxhook->loaderGet())
		.loaderRegister(Stringkey(".py"),m_pythonhook->loaderGet());
	}

Session& Session::configClear()
	{
	m_targetinfo.clear();
	m_source_files.clear();
	m_dirloader.configClear();
	m_cxxhook->configClear();
	m_pythonhook->configClear();
	m_graph_dirty=1;
	return *this;
	}

Session& Session::configAppend(const ResourceObject& maikeconfig)
	{
	if(maikeconfig.objectExists("session"))
		{
		auto session=maikeconfig.objectGet("session");
		if(session.objectExists("source_files"))
			{
			auto source_files=session.objectGet("source_files");
			auto N_sources=source_files.objectCountGet();
			for(decltype(N_sources) k=0;k<N_sources;++k)
				{
				sourceFileAppend(static_cast<const char*>(source_files.objectGet(k)));
				}
			}
		}

	if(maikeconfig.objectExists("targetinfo"))
		{m_targetinfo.configAppend(maikeconfig.objectGet("targetinfo"));}

	if(maikeconfig.objectExists("directoryoptions"))
		{m_dirloader.configAppend(maikeconfig.objectGet("directoryoptions"));}

	if(maikeconfig.objectExists("cxxoptions"))
		{m_cxxhook->configAppend(maikeconfig.objectGet("cxxoptions"));}

	if(maikeconfig.objectExists("pythonoptions"))
		{m_pythonhook->configAppend(maikeconfig.objectGet("pythonoptions"));}

	m_graph_dirty=1;
	return *this;
	}

void Session::configDump(ResourceObject& maikeconfig) const
	{
		{
		ResourceObject session(ResourceObject::Type::OBJECT);
			{
			ResourceObject source_files(ResourceObject::Type::ARRAY);
			auto ptr=m_source_files.data();
			auto ptr_end=ptr+m_source_files.size();
			while(ptr!=ptr_end)
				{
				source_files.objectAppend(ResourceObject(ptr->c_str()));
				++ptr;
				}
			session.objectSet("source_files",std::move(source_files));
			}
		maikeconfig.objectSet("session",std::move(session));
		}

		{
		ResourceObject targetinfo(ResourceObject::Type::OBJECT);
		m_targetinfo.configDump(targetinfo);
		maikeconfig.objectSet("targetinfo",std::move(targetinfo));
		}

		{
		ResourceObject directoryoptions(ResourceObject::Type::OBJECT);
		m_dirloader.configDump(directoryoptions);
		maikeconfig.objectSet("directoryoptions",std::move(directoryoptions));
		}

		{
		ResourceObject pythonoptions(ResourceObject::Type::OBJECT);
		m_pythonhook->configDump(pythonoptions);
		maikeconfig.objectSet("pythonoptions",std::move(pythonoptions));
		}

		{
		ResourceObject cxxoptions(ResourceObject::Type::OBJECT);
		m_cxxhook->configDump(cxxoptions);
		maikeconfig.objectSet("cxxoptions",std::move(cxxoptions));
		}
	}

Session& Session::sourceFileAppend(const char* filename)
	{
	m_source_files.push_back(std::string(filename));
	m_graph_dirty=1;
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
		{
		auto t=m_graph.targetFind(Stringkey(filename));
		if(dynamic_cast<TargetDirectory*>(t))
			{m_graph.targetsRemove(ByParentDirectory(t->sourceNameGet()));}
		}
	m_graph.targetsRemove(BySourceName(filename));
	m_spider.scanFile(filename,"").run();
	m_graph_dirty=0;
	return *this;
	}

Session& Session::dependenciesClear() noexcept
	{
	m_graph.clear();
	m_graph_dirty=0;
	return *this;
	}

Session& Session::dependenciesReload()
	{
	dependenciesClear();
	auto i=m_source_files.data();
	auto i_end=i + m_source_files.size();
	while(i!=i_end)
		{
		m_spider.scanFile(i->c_str(),"");
		++i;
		}
	m_spider.run();
	m_graph_dirty=0;
	return *this;
	}

void Session::targetsProcess(DependencyGraph::TargetProcessor&& proc)
	{
	if(m_graph_dirty)
		{dependenciesReload();}

	m_graph.targetsProcess(std::move(proc));
	}

void Session::targetsProcess(DependencyGraph::TargetProcessorConst&& proc) const
	{
	if(m_graph_dirty)
		{const_cast<Session*>(this)->dependenciesReload();}

	m_graph.targetsProcess(std::move(proc));
	}

const Target& Session::target(const char* name) const
	{
	auto ret=m_graph.targetFind(Stringkey(name));
	if(ret==nullptr)
		{exceptionRaise(ErrorMessage("Target #0; has not been loaded.",{name}));}
	return *ret;
	}

Target& Session::target(const char* name)
	{
	auto ret=m_graph.targetFind(Stringkey(name));
	if(ret==nullptr)
		{exceptionRaise(ErrorMessage("Target #0; has not been loaded.",{name}));}
	return *ret;
	}
