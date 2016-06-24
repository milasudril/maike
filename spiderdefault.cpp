//@	{
//@	"targets":[{"name":"spiderdafault.o","type":"object"}]
//@	}

#include "spiderdefault.hpp"
#include "directorylister.hpp"
#include "fileinfo.hpp"
#include "targetloader.hpp"

#include <cstdio>

using namespace Maike;

SpiderDefault::SpiderDefault(const std::map<Stringkey,const TargetLoader*>& loaders
	,DependencyGraph& targets):r_loaders(loaders),r_targets(targets)
	{
	}



SpiderDefault& SpiderDefault::scanFile(const char* filename)
	{
	if(m_files_visited.find(VisitedKey(filename))==m_files_visited.end())
		{m_files_to_scan.push(filename);}
	return *this;
	}



static const TargetLoader* targetLoaderGet(const Stringkey& key
	,const std::map<Stringkey,const TargetLoader*>& loaders)
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
		auto filename=std::move(m_files_to_scan.top());
		m_files_to_scan.pop();

		auto loader=targetLoaderGet(targetLoaderKeyGet(filename),r_loaders);
		if(loader!=nullptr)
			{loader->targetsLoad(filename.c_str(),*this,r_targets);}
		m_files_visited.insert(VisitedKey(filename.c_str()));
		}

	return *this;
	}

const char* SpiderDefault::targetPrefixGet() const noexcept
	{return nullptr;}

const char* SpiderDefault::sourcePrefixGet() const noexcept
	{return nullptr;}
