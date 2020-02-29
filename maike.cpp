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
		Maike::ConfigStore settings() const {return Maike::ConfigStore{};}
		void settings(Maike::ConfigStore const&){}
};

struct SourceFileByName
{
	bool operator()(Maike::SourceFile const& a, Maike::SourceFile const& b)
	{
		return a.name() < b.name();
	}
};

std::optional<Maike::SourceFile> loadSourceFile(Maike::fs::path&& path, const Maike::fs::path& target_dir)
{
	std::vector<Maike::Dependency> deps{Maike::Dependency{path.parent_path(), Maike::Dependency::Resolver::InternalLookup}};
	std::vector<Maike::fs::path> targets;

	if(is_directory(path))
	{
		targets.push_back(target_dir/path);
		return Maike::SourceFile{std::move(path), std::move(deps), std::move(targets), Maike::Compiler{MkDir{}}};
	}

	return std::optional<Maike::SourceFile>{};
}

void visitChildren(Maike::SourceFile const& src_file, std::stack<Maike::fs::path>& paths_to_visit)
{
	if(is_directory(src_file.name()))
	{
		auto i = Maike::fs::directory_iterator{src_file.name()};
		std::for_each(begin(i), end(i), [&paths_to_visit](auto const& item){
			paths_to_visit.push(item.path());
		});
	}
}

int main()
{
	// Config stuff
	std::vector<std::regex> input_filters;
	input_filters.push_back(std::regex{"/\\..*", std::regex_constants::basic});
	input_filters.push_back(std::regex{"/__*.*", std::regex_constants::basic});

	Maike::fs::path target_dir{"__targets_new"};


	// Current state
	std::set<Maike::SourceFile, SourceFileByName> sources;
	std::set<Maike::fs::path> loaded_targets;


	auto skip = [](auto const& path, auto const& regex_list){
		return std::any_of(std::begin(regex_list), std::end(regex_list), [&path](auto const& regex){
			return regex_search(path.c_str(), regex);
		});
	};

	// Loader
	std::stack<Maike::fs::path> paths_to_visit;
	paths_to_visit.push(Maike::fs::path{"."});
	while(!paths_to_visit.empty())
	{
		auto path = pop(paths_to_visit);
		if(skip(path, input_filters))
		{ continue; }

		if(auto src_file = loadSourceFile(std::move(path), target_dir); src_file.has_value())
		{
			auto const& targets = src_file->targets();
			std::for_each(std::begin(targets), std::end(targets), [&loaded_targets](auto const& item) {
				auto i = loaded_targets.insert(item);
				if(!i.second)
				{
					throw std::runtime_error{"Target has already been defined"};
				}
			});

			visitChildren(*src_file, paths_to_visit);
			sources.insert(std::move(*src_file));
		}
	}

	std::for_each(std::begin(sources), std::end(sources), [](auto const& item) {
		printf("%s ->", item.name().c_str());
		auto const& targets = item.targets();
		std::for_each(std::begin(targets), std::end(targets), [](auto const& item)
		{
			printf(" %s", item.c_str());
		});
		putchar('\n');
	});
}