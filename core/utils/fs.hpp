//@	{
//@	 "dependencies_extra":[{"ref":"./fs.o","rel":"implementation"}]
//@	 }

#ifndef MAIKE_FS_HPP
#define MAIKE_FS_HPP

#include <filesystem>

namespace Maike
{
	namespace fs = std::filesystem;

	bool isNewer(fs::path const& a, fs::path const& b);

	bool isOlder(fs::path const& a, fs::path const& b);

	fs::path const& execPrefix();

	inline fs::path makeShellExecPath(fs::path const& orig)
	{
		if(orig.parent_path() != "") { return orig; }
		return fs::path{"."} / orig;
	}

	std::string getFullExtension(fs::path const& path);

	template<class Dictionary>
	auto matchLongestExtension(fs::path const& path, Dictionary const& dict)
	{
		auto extension = getFullExtension(path);
		auto range = std::string_view{extension};
		auto ptr = std::begin(range);
		auto n = std::size(range);
		while(ptr != std::end(range))
		{
			if(*ptr == '.')
			{
				auto i = dict.find(std::string_view{ptr, n});
				if(i != std::end(dict)) { return i; }
			}
			++ptr;
			--n;
		}
		return std::end(dict);
	}
}

#endif
