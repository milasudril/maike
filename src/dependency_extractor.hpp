//@	{
//@	  "targets":[{"name":"dependency_extractor.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_DEPENDENCYEXTRACTOR_HPP
#define MAIKE_DEPENDENCYEXTRACTOR_HPP

#include "./reader.hpp"
#include "./source_file_info.hpp"

#include <type_traits>
#include <vector>

namespace Maike
{
	namespace dependency_extractor_detail
	{
		template<class T>
		void do_run(T const& self, Reader input, std::vector<Dependency>& deps)
		{
			run(self, input, deps);
		}
	};

	class DependencyExtractor
	{
		public:
			template<class Extractor, std::enable_if_t<!std::is_same_v<Extractor, DependencyExtractor>, int> = 0>
			DependencyExtractor(Extractor const&& extractor) = delete;

			template<class Extractor, std::enable_if_t<!std::is_same_v<Extractor, DependencyExtractor>, int> = 0>
			explicit DependencyExtractor(Extractor const& extractor):
				r_extractor{&extractor},
				r_callback{[](void const* extractor, Reader input, std::vector<Dependency>& deps){
					auto const& self = *reinterpret_cast<Extractor const*>(extractor);
					dependency_extractor_detail::do_run(self, input, deps);
				}}
			{}

			void run(Reader input, std::vector<Dependency>& deps) const
			{
				r_callback(r_extractor, input, deps);
			}

		private:
			void const* r_extractor;
			void (*r_callback)(void const* extractor, Reader input, std::vector<Dependency>& deps);
	};

	inline void run(DependencyExtractor extractor, Reader input, std::vector<Dependency>& deps)
	{ extractor.run(input, deps); }
}

#endif