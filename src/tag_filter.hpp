//@	{
//@	  "targets":[{"name":"tag_filter.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_TAGFILTER_HPP
#define MAIKE_TAGFILTER_HPP

#include "./reader.hpp"
#include "./writer.hpp"

#include <type_traits>

namespace Maike
{
	enum class TagFilterOutput{Source, Tags};

	using SourceOutStream = TaggedWriter<TagFilterOutput::Source>;
	using TagOutStream = TaggedWriter<TagFilterOutput::Tags>;

	namespace tag_filter_detail
	{
		template<class T>
		void do_run(T const& self, Reader input, SourceOutStream source, TagOutStream tags)
		{
			run(self, input, source, tags);
		}
	};

	class TagFilter
	{
		public:
			template<class Filter, std::enable_if_t<!std::is_same_v<Filter, TagFilter>, int> = 0>
			TagFilter(Filter const&& filter) = delete;

			template<class Filter, std::enable_if_t<!std::is_same_v<Filter, TagFilter>, int> = 0>
			explicit TagFilter(Filter const& filter):
				r_filter{&filter},
				r_callback{[](void const* filter, Reader input, SourceOutStream source, TagOutStream tags){
					auto const& self = *reinterpret_cast<Filter const*>(filter);
					tag_filter_detail::do_run(self, input, source, tags);
				}}
			{}

			void run(Reader input, SourceOutStream source, TagOutStream tags) const
			{
				r_callback(r_filter, input, source, tags);
			}

		private:
			void const* r_filter;
			void (*r_callback)(void const* filter, Reader input, SourceOutStream source, TagOutStream tags);
	};

	inline void run(TagFilter filter, Reader input, SourceOutStream source, TagOutStream tags)
	{filter.run(input, source, tags);}
}

#endif