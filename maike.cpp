//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "sourcefile.hpp"

template<class T>
decltype(auto) pop(T& stack)
{
	auto ret = std::move(stack.top());
	stack.pop();
	return ret;
}

int main()
{
	std::stack<Maike::Path> paths_to_visit;
	paths_to_visit.push(Maike::Path{"."});
	while(!paths_to_visit.empty())
	{
		Maike::SourceFile test{pop(paths_to_visit), paths_to_visit};
	}
}