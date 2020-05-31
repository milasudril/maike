//@	{
//@	  "targets":[{"name":"loader.hpp","type":"include"}]
//@	 }

#ifndef MAIKE_SOURCEFILEINFOLOADERS_LOADER_HPP
#define MAIKE_SOURCEFILEINFOLOADERS_LOADER_HPP

#include "src/io/reader.hpp"
#include "src/io/writer.hpp"
#include "src/db/source_file_info.hpp"
#include "src/key_value_store/compound.hpp"

#include "src/compiler.hpp"

#include <type_traits>
#include <cassert>

namespace Maike::SourceFileInfoLoaders
{
	enum class TagFilterOutput
	{
		Source,
		Tags
	};

	using SourceOutStream = Io::TaggedWriter<TagFilterOutput::Source>;
	using TagsOutStream = Io::TaggedWriter<TagFilterOutput::Tags>;

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
			   filter_input{
			      [](void const* handle, Io::Reader input, SourceOutStream src, TagsOutStream tags) {
				      auto const& self = *reinterpret_cast<T const*>(handle);
				      filterInput(self, input, src, tags);
			      }},
			   get_dependencies{[](void const* handle, Io::Reader input) {
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
			   }},
			   to_json{[](void const* handle) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return toJson(self);
			   }}
			{
			}

			void (*filter_input)(void const* handle,
			                     Io::Reader input,
			                     SourceOutStream source,
			                     TagsOutStream tags);
			std::vector<Db::UnresolvedDependency> (*get_dependencies)(void const* handle,
			                                                          Io::Reader source_stream);
			Compiler (*get_compiler)(void const* handle, KeyValueStore::CompoundRefConst cfg);
			void (*destroy)(void* handle);
			KeyValueStore::JsonHandle (*to_json)(void const* handle);
		};
	}

	class Loader
	{
	public:
		Loader(Loader&& other) noexcept: m_handle{other.m_handle}, m_vtable{other.m_vtable}
		{
			other.m_handle = nullptr;
		}

		Loader& operator=(Loader&& other) noexcept
		{
			std::swap(m_handle, other.m_handle);
			std::swap(m_vtable, other.m_vtable);
			other.m_vtable.destroy(other.m_handle);
			other.m_handle = nullptr;
			return *this;
		}

		~Loader()
		{
			m_vtable.destroy(m_handle);
		}

		template<class LoaderType,
		         std::enable_if_t<!std::is_same_v<std::decay_t<LoaderType>, Loader>, int> = 0>
		explicit Loader(LoaderType&& loader):
		   m_handle{new LoaderType(std::forward<LoaderType>(loader))},
		   m_vtable{source_file_loader_detail::Tag<LoaderType>{}}
		{
		}

		template<class LoaderType,
		         std::enable_if_t<!std::is_same_v<std::decay_t<LoaderType>, Loader>, int> = 0>
		explicit Loader(std::unique_ptr<LoaderType> loader):
		   m_handle{loader.release()},
		   m_vtable{source_file_loader_detail::Tag<LoaderType>{}}
		{
		}

		void filterInput(Io::Reader input, SourceOutStream source, TagsOutStream tags) const
		{
			assert(valid());
			m_vtable.filter_input(m_handle, input, source, tags);
		}

		std::vector<Db::UnresolvedDependency> getDependencies(Io::Reader input) const
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

		KeyValueStore::JsonHandle toJson() const
		{
			return m_vtable.to_json(m_handle);
		}


	private:
		void* m_handle;
		source_file_loader_detail::Vtable m_vtable;
	};

	inline auto toJson(Loader const& loader)
	{
		return loader.toJson();
	}
};

#endif