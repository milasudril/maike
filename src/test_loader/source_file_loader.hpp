//@	{
//@	  "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	 }

#include "src/compiler.hpp"
#include "src/reader.hpp"
#include "src/tag_filter.hpp"
#include "src/source_file_info.hpp"
#include "src/key_value_store/compound.hpp"
#include "src/input_buffer.hpp"
#include "src/output_buffer.hpp"

#include <vector>

namespace Test
{
	class SourceFileLoader
	{
	public:
		class DependencyMarkers
		{
		public:
			constexpr DependencyMarkers(): m_internal{}, m_none{}
			{
			}

			constexpr DependencyMarkers& internal(char x)
			{
				m_internal = x;
				return *this;
			}

			constexpr DependencyMarkers& none(char x)
			{
				m_none = x;
				return *this;
			}

			constexpr auto internal() const
			{
				return m_internal;
			}

			constexpr auto none() const
			{
				return m_none;
			}

		private:
			char m_internal;
			char m_none;
		};

		constexpr explicit SourceFileLoader(char tag_prefix, DependencyMarkers dep_markers):
		   m_tag_prefix{tag_prefix},
		   m_dep_markers{dep_markers}
		{
		}

		constexpr auto tagPrefix() const
		{
			return m_tag_prefix;
		}

		constexpr auto depMarkers() const
		{
			return m_dep_markers;
		}

	private:
		char m_tag_prefix;
		DependencyMarkers m_dep_markers;
	};

	void filterInput(SourceFileLoader const& loader,
	                 Maike::Reader input,
	                 Maike::SourceOutStream src,
	                 Maike::TagsOutStream tags)
	{
		Maike::InputBuffer input_buff{input};
		Maike::OutputBuffer src_buff{src};
		Maike::OutputBuffer tags_buff{tags};

		enum class State : int
		{
			Newline,
			Tag,
			Source
		};
		auto state = State::Newline;
		while(true)
		{
			auto ch_in_ = input_buff.getchar();
			if(ch_in_ == -1) { return; }

			auto ch_in = static_cast<char>(ch_in_);

			switch(state)
			{
				case State::Newline:
					if(ch_in == loader.tagPrefix())
					{
						state = State::Tag;
						src_buff.putchar('\n');
					}
					else if(!(ch_in >= '\0' && ch_in <= ' '))
					{
						state = State::Source;
						src_buff.putchar(ch_in);
						tags_buff.putchar('\n');
					}
					break;

				case State::Tag:
					switch(ch_in)
					{
						case '\n':
							state = State::Newline;
							tags_buff.putchar(ch_in);
							break;
						default: tags_buff.putchar(ch_in); break;
					}
					break;
				case State::Source:
					switch(ch_in)
					{
						case '\n':
							state = State::Newline;
							src_buff.putchar(ch_in);
							break;
						default: src_buff.putchar(ch_in); break;
					}
			}
		}
	}

	std::vector<Maike::Dependency> loadDependencies(SourceFileLoader const& loader, Maike::Reader src)
	{
		std::vector<Maike::Dependency> ret;
		Maike::InputBuffer src_buff{src};

		Maike::fs::path buffer;
		Maike::Dependency::Resolver resolver;

		enum class State : int
		{
			Newline,
			Token
		};

		auto state = State::Newline;

		while(true)
		{
			auto ch_in_ = src_buff.getchar();
			if(ch_in_ == -1) { return ret; }

			auto ch_in = static_cast<char>(ch_in_);

			switch(state)
			{
				case State::Newline:
					if(ch_in == loader.depMarkers().none())
					{
						state = State::Token;
						resolver = Maike::Dependency::Resolver::None;
					}
					else if(ch_in == loader.depMarkers().internal())
					{
						state = State::Token;
						resolver = Maike::Dependency::Resolver::InternalLookup;
					}
					else if(!(ch_in >= '\0' && ch_in <= ' '))
					{
						throw std::runtime_error{"Parse error"};
					}

					break;

				case State::Token:
					switch(ch_in)
					{
						case '\n':
							state = State::Newline;
							ret.push_back(Maike::Dependency{std::move(buffer), resolver});
							buffer.clear();
							break;
						default: buffer += ch_in;
					}
					break;
			}
		}
		return ret;
	}
}
#if 0
			template<class T>
			Vtable(Tag<T>):
			   filter_input{[](void const* handle, Reader input, SourceOutStream src, TagsOutStream tags) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   filterInput(self, input, src, tags);
			   }},
			   load_dependencies{[](void const* handle, Reader input) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return loadDependencies(self, input);
			   }},
			   get_compiler{[](void const* handle, KeyValueStore::CompoundRefConst cfg) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return getCompiler(self, cfg);
			   }},

#endif