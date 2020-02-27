//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "sourcefile.hpp"

template<class T>
decltype(auto) pop(T& stack)
{
	auto ret = std::move(stack.top());
	stack.pop();
	return ret;
}

namespace fs = std::experimental::filesystem;

int main()
{
	std::stack<Maike::Path> paths_to_visit;
	paths_to_visit.push(Maike::Path{"."});
	while(!paths_to_visit.empty())
	{
		Maike::SourceFile test{pop(paths_to_visit)};

		if(is_directory(test.name()))
		{
			auto i = fs::directory_iterator(test.name());
			std::for_each(begin(i), end(i), [&paths_to_visit](auto const& item) {
				paths_to_visit.push(item.path());
			});
		}
	}
}