//@	 {"targets":[{"name":"sourcefile.o","type":"object","dependencies":[{"ref":"stdc++fs", "rel":"external"}]}]}

#include "./sourcefile.hpp"

namespace fs = std::experimental::filesystem;

Maike::SourceFile::SourceFile(Path&& src, std::stack<Path>& paths_to_visist)
{
	printf("%s\n", src.c_str());
	if(is_directory(src))
	{
		auto i = fs::directory_iterator(src);
		std::for_each(begin(i), end(i), [&paths_to_visist](auto const& item){
			paths_to_visist.push(item.path());
		});
	}
}
