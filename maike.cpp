//@	 {"targets":[{"name":"maike_next","type":"application"}]}

#include "./sourcefile.hpp"
#include <set>
#include <regex>

template<class T>
decltype(auto) pop(T& stack)
{
	auto ret = std::move(stack.top());
	stack.pop();
	return ret;
}

class MkDir
{
	public:
		int run(Maike::fs::path const&,
                     std::vector<Maike::fs::path const*> const&,
                     std::vector<Maike::fs::path const*> const&,
                     Maike::CompilationLog&) const
                     {return 0;}
		Maike::DataStore settings() const {return Maike::DataStore{};}
		void settings(Maike::DataStore const&){}
};

int main()
{
	std::vector<std::regex> input_filters;
	input_filters.push_back(std::regex{"/\\..*", std::regex_constants::basic});
	input_filters.push_back(std::regex{"/__*.*", std::regex_constants::basic});

	Maike::fs::path output_dir{"__targets_new"};

	auto skip = [](auto const& path, auto const& regex_list){
		return std::any_of(std::begin(regex_list), std::end(regex_list), [&path](auto const& regex){
			return regex_search(path.c_str(), regex);
		});
	};

	std::stack<Maike::fs::path> paths_to_visit;
	paths_to_visit.push(Maike::fs::path{"."});
	while(!paths_to_visit.empty())
	{
		auto path = pop(paths_to_visit);
		if(skip(path, input_filters))
		{ continue; }

		printf("%s %s\n", path.parent_path().c_str(), path.c_str());
		std::vector<Maike::Dependency> deps;
		deps.push_back(Maike::Dependency{path.parent_path(), Maike::Dependency::Resolver::InternalLookup});
		if(is_directory(path))
		{
			Maike::SourceFile src{std::move(path),
			                      std::move(deps),
			                      std::vector<Maike::fs::path>{output_dir/path},
			                      Maike::Compiler{MkDir{}}};

			auto i = Maike::fs::directory_iterator{src.name()};
			std::for_each(begin(i), end(i), [&paths_to_visit](auto const& item){
				paths_to_visit.push(item.path());
			});
		}
		else
		{
		}
	}
}