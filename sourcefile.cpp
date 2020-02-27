//@	 {"targets":[{"name":"sourcefile.o","type":"object","dependencies":[{"ref":"stdc++fs", "rel":"external"}]}]}

#include "./sourcefile.hpp"

namespace fs = std::experimental::filesystem;

Maike::SourceFile::SourceFile(Path&& src)
{
	if(is_directory(src))
	{
		auto i = fs::directory_iterator(src);
		std::for_each(begin(i), end(i), [](auto const& item){
			printf("%s\n", item.path().c_str());
		});
	}
}
