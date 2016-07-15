//@	{
//@	"targets":[{"name":"targetdirectoryloader.o","type":"object"}]
//@	}

#include "targetdirectoryloader.hpp"
#include "directorylister.hpp"
#include "fileinfo.hpp"
#include "spider.hpp"
#include "targetdirectory.hpp"
#include "dependencygraph.hpp"
#include "resourceobject.hpp"
#include "handle.hpp"
#include "target_factorydelegator.hpp"
#include <string>
#include <cstring>


using namespace Maike;

TargetDirectoryLoader::TargetDirectoryLoader()
	{
	configClear();
	}

void TargetDirectoryLoader::targetsLoad(const char* name_src,const char* in_dir
	,Spider& spider,DependencyGraph& graph,Target_FactoryDelegator& factory) const
	{
	Handle<TargetDirectory> target(
		TargetDirectory::create(name_src,in_dir,factory.idGet()));
	graph.targetRegister(target);
	DirectoryLister dirlister(name_src);
	const char* entry=dirlister.read();
	while(entry!=nullptr)
		{
		if(m_ignore.find(Stringkey(entry))==m_ignore.end())
			{
			std::string path_tot;
			path_tot+=name_src;
			path_tot+='/';
			path_tot+=entry;
			auto entry_type=FileInfo(path_tot.c_str()).typeGet();
			if((entry_type==FileInfo::Type::DIRECTORY && m_recursive)
				|| entry_type==FileInfo::Type::FILE)
				{spider.scanFile(path_tot.c_str(),name_src);}
			}

		entry=dirlister.read();
		}
	}

TargetDirectoryLoader& TargetDirectoryLoader::pathReject(const Stringkey& key)
	{
	m_ignore.insert(key);
	return *this;
	}

TargetDirectoryLoader& TargetDirectoryLoader::pathAccept(const Stringkey& key)
	{
	m_ignore.erase(key);
	return *this;
	}

void TargetDirectoryLoader::configClear()
	{
	m_ignore.clear();
	m_recursive=0;
//	IRP (Infinite Recursion Prevention)
	pathReject(Stringkey(".")); //Remove references to this
	pathReject(Stringkey(".."));//and parent directory
	}

TargetDirectoryLoader& TargetDirectoryLoader::configAppendDefault()
	{
	m_recursive=1;
	pathReject(Stringkey("."));
	pathReject(Stringkey(".."));
	return *this;
	}

TargetDirectoryLoader& TargetDirectoryLoader::configAppend(const ResourceObject& directoryoptions)
	{
	if(directoryoptions.objectExists("recursive"))
		{
		m_recursive=static_cast<long long int>(directoryoptions.objectGet("recursive") );
		}


	if(directoryoptions.objectExists("paths_reject"))
		{
		auto paths_reject=directoryoptions.objectGet("paths_reject");
		auto N=paths_reject.objectCountGet();
		for(decltype(N) k=0;k<N;++k)
			{
			auto path=static_cast<const char*>(paths_reject.objectGet(k));
			pathReject(Stringkey(path));
			}
		}

	return *this;
	}


void TargetDirectoryLoader::configDump(ResourceObject& directoryoptions) const
	{
	directoryoptions.objectSet("recursive",ResourceObject(static_cast<long long int>(m_recursive)));
	}
