//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "sourcefile.hpp"
#include <set>
#include <regex>

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
	std::vector<std::regex> ignore;
	ignore.push_back(std::regex{"/\\..*", std::regex_constants::basic});
	ignore.push_back(std::regex{"/__*.*", std::regex_constants::basic});
	paths_to_visit.push(Maike::Path{"."});
	while(!paths_to_visit.empty())
	{
		Maike::SourceFile test{pop(paths_to_visit)};

		if(is_directory(test.name()))
		{
			auto i = fs::directory_iterator(test.name());
			std::for_each(begin(i), end(i), [&paths_to_visit, &ignore](auto const& item) {
				if(std::none_of(std::begin(ignore), std::end(ignore), [&item](auto const& regex){
					return regex_search(item.path().c_str(), regex);
				}))
				{
					paths_to_visit.push(item.path());
				}
			});
		}
	}
}