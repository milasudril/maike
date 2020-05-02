//@	{
//@	  "targets":[{"name":"source_file_loader.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_SOURCEFILELOADER_HPP
#define MAIKE_SOURCEFILELOADER_HPP

#include "src/compiler.hpp"
#include "src/reader.hpp"
#include "src/writer.hpp"
#include "src/source_file_info.hpp"
#include "src/key_value_store/compound.hpp"

#include <type_traits>
#include <cassert>

namespace Maike
{
	enum class TagFilterOutput
	{
		Source,
		Tags
	};

	using SourceOutStream = TaggedWriter<TagFilterOutput::Source>;
	using TagsOutStream = TaggedWriter<TagFilterOutput::Tags>;

	namespace source_file_loader_detail
	{
		template<class T>
		struct Tag
		{
		};

		struct Vtable
		{
			template<class T>
			Vtable(Tag<T>):
			   filter_input{[](void const* handle, Reader input, SourceOutStream src, TagsOutStream tags) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   filterInput(self, input, src, tags);
			   }},
			   get_dependencies{[](void const* handle, Reader input) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return getDependencies(self, input);
			   }},
			   get_compiler{[](void const* handle, KeyValueStore::CompoundRefConst cfg) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return getCompiler(self, cfg);
			   }},
			   destroy{[](void* handle) {
				   auto self = reinterpret_cast<T*>(handle);
				   delete self;
			   }}
			{
			}

			void (*filter_input)(void const* handle,
			                     Reader input,
			                     SourceOutStream source,
			                     TagsOutStream tags);
			std::vector<Dependency> (*get_dependencies)(void const* handle, Reader source_stream);
			Compiler (*get_compiler)(void const* handle, KeyValueStore::CompoundRefConst cfg);
			void (*destroy)(void* handle);
		};
	}

	class SourceFileLoader
	{
	public:
		SourceFileLoader(SourceFileLoader&& other) noexcept:
		   m_handle{other.m_handle},
		   m_vtable{other.m_vtable}
		{
			other.m_handle = nullptr;
		}

		SourceFileLoader& operator=(SourceFileLoader&& other) noexcept
		{
			std::swap(m_handle, other.m_handle);
			std::swap(m_vtable, other.m_vtable);
			other.m_vtable.destroy(other.m_handle);
			other.m_handle = nullptr;
			return *this;
		}

		~SourceFileLoader()
		{
			m_vtable.destroy(m_handle);
		}

		template<class Loader,
		         std::enable_if_t<!std::is_same_v<std::decay_t<Loader>, SourceFileLoader>, int> = 0>
		explicit SourceFileLoader(Loader&& loader):
		   m_handle{new Loader(std::forward<Loader>(loader))},
		   m_vtable{source_file_loader_detail::Tag<Loader>{}}
		{
		}

		template<class Loader,
		         std::enable_if_t<!std::is_same_v<std::decay_t<Loader>, SourceFileLoader>, int> = 0>
		explicit SourceFileLoader(std::unique_ptr<Loader> loader):
		   m_handle{loader.release()},
		   m_vtable{source_file_loader_detail::Tag<Loader>{}}
		{
		}

		void filterInput(Reader input, SourceOutStream source, TagsOutStream tags) const
		{
			assert(valid());
			m_vtable.filter_input(m_handle, input, source, tags);
		}

		std::vector<Dependency> getDependencies(Reader input) const
		{
			assert(valid());
			return m_vtable.get_dependencies(m_handle, input);
		}

		Compiler getCompiler(KeyValueStore::CompoundRefConst cfg) const
		{
			assert(valid());
			return m_vtable.get_compiler(m_handle, cfg);
		}

		Compiler getCompiler() const
		{
			KeyValueStore::Compound empty;
			return getCompiler(empty.reference());
		}

		bool valid() const
		{
			return m_handle != nullptr;
		}

	private:
		void* m_handle;
		source_file_loader_detail::Vtable m_vtable;
	};
};

#endif