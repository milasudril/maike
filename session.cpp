//@	{"targets":[{"name":"session.o","type":"object"}]}

#include "session.hpp"
#include "resourceobject.hpp"
#include "targetdirectory.hpp"
#include <cstring>

using namespace Maike;

Session::Session(const ResourceObject& maikeconfig):
	 m_targetinfo(maikeconfig.objectGet("targetinfo"))
	,m_dirloader(maikeconfig.objectGet("directoryoptions"))
	,m_cxxhook(TargetCxxHook::create(maikeconfig.objectGet("cxxoptions"),m_targetinfo))
	,m_pythonhook(TargetPythonHook::create(maikeconfig.objectGet("pythonoptions"),m_targetinfo))
	,m_evaluator(m_targetinfo)
	,m_delegator(static_cast<const char*>(m_targetinfo.variableGet(Stringkey("target_directory")))
		,m_evaluator,m_id_gen)
	,m_graph(m_id_gen)
	,m_spider(m_delegator,m_graph)
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
	return *this;
	}
