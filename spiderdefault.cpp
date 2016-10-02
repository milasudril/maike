//@	{
//@	"targets":[{"name":"spiderdefault.o","type":"object"}]
//@	}

#include "spiderdefault.hpp"
#include "directorylister.hpp"
#include "fileinfo.hpp"
#include "target_loader.hpp"

using namespace Maike;

SpiderDefault::SpiderDefault(Target_FactoryDelegator& target_creator,DependencyGraph& targets):
	r_target_creator(target_creator),r_targets(targets)
	{
	}



SpiderDefault& SpiderDefault::scanFile(const char* filename,const char* in_dir)
	{
	if(m_files_visited.find(VisitedKey(filename))==m_files_visited.end())
		{
		m_files_to_scan.push({filename,in_dir});
		m_files_visited.insert(VisitedKey(filename));
		}
	return *this;
	}

SpiderDefault& SpiderDefault::loaderRegister(const Stringkey& filename_ext
	,const Target_Loader& loader)
	{
	m_r_loaders[filename_ext]=&loader;
	return *this;
	}

void SpiderDefault::loadersUnregister() noexcept
	{
	m_r_loaders.clear();
	}

static const Target_Loader* targetLoaderGet(const Stringkey& key
	,const std::map<Stringkey,const Target_Loader*>& loaders)
	{
	auto i=loaders.find(key);
	if(i==loaders.end())
		{
		return nullptr;
		}
	return i->second;
	}

static Stringkey targetLoaderKeyGet(const std::string& filename)
	{
	switch(FileInfo(filename.c_str()).typeGet())
		{
		case FileInfo::Type::FILE:
			{
			auto pos=filename.find_last_of('.');
			if(pos==std::string::npos)
				{return Stringkey("");}
			return Stringkey(&filename[pos]);
			}

		case FileInfo::Type::DIRECTORY:
			return Stringkey(".");

		default:
			return Stringkey("");
		}
	}

SpiderDefault& SpiderDefault::run()
	{
	while(!m_files_to_scan.empty())
		{
		auto p=std::move(m_files_to_scan.top());
		m_files_to_scan.pop();
		auto filename=p.first.c_str();
		auto loader=targetLoaderGet(targetLoaderKeyGet(p.first),m_r_loaders);
		if(loader!=nullptr)
			{
			auto in_dir=p.second.c_str();
			loader->targetsLoad(filename,in_dir,*this,r_targets,r_target_creator);
			}
		}

	return *this;
	}