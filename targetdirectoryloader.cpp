//@	{
//@	"targets":[{"name":"targetdirectoryloader.o","type":"object"}]
//@	}

#include "targetdirectoryloader.hpp"
#include "directorylister.hpp"
#include "fileinfo.hpp"
#include "spider.hpp"
#include <string>

#include <cstdio>

using namespace Maike;

TargetDirectoryLoader::TargetDirectoryLoader():m_recursive(1)
	{
	m_ignore.insert(Stringkey("."));
	m_ignore.insert(Stringkey(".."));
	}

void TargetDirectoryLoader::targetsLoad(const char* name_src,Spider& spider
	,DependencyGraph& graph) const
	{
//	auto target=new TargetDirectory();
//	graph.targetRegister(std::unique_ptr<Target>(target));
	DirectoryLister dirlister(name_src);
	const char* entry=dirlister.read();
	while(entry!=nullptr)
		{
		if(m_ignore.find(Stringkey(entry))==m_ignore.end())
			{
			std::string path_tot(name_src);
			path_tot+='/';
			path_tot+=entry;
			auto entry_type=FileInfo(path_tot.c_str()).typeGet();
			if((entry_type==FileInfo::Type::DIRECTORY && m_recursive)
				|| entry_type==FileInfo::Type::FILE)
				{
				printf("%s\n",path_tot.c_str());
				spider.scanFile(path_tot.c_str());
				}
			}

		entry=dirlister.read();
		}
	}
