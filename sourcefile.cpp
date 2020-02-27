//@	 {"targets":[{"name":"sourcefile.o","type":"object","dependencies":[{"ref":"stdc++fs", "rel":"external"}]}]}

#include "./sourcefile.hpp"

Maike::SourceFile::SourceFile(Path&& src):
m_name{std::move(src)}
{
	printf("%s\n", m_name.c_str());
}
