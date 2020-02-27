//@	 {"targets":[{"name":"sourcefile.o","type":"object","dependencies":[{"ref":"stdc++fs", "rel":"external"}]}]}

#include "./sourcefile.hpp"

namespace
{
	std::string getExtension(Maike::Path const& path)
	{
		auto filename = path.filename().c_str();
		while(true)
		{
			auto ch_in = *filename;
			if(ch_in == 0)
			{
				return "";
			}
			if(ch_in == '.')
			{
				return filename;
			}
			++filename;
		}
	}
}

Maike::SourceFile::SourceFile(Path&& src):
m_name{std::move(src)}
{
	printf("%s  %s\n", m_name.c_str(), getExtension(m_name).c_str());
}
