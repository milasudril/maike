//@	{
//@	 }

#ifndef MAIKE_SOURCEFILEINFOLOADERS_LOADER_HPP
#define MAIKE_SOURCEFILEINFOLOADERS_LOADER_HPP

#include "core/io/reader.hpp"
#include "core/io/writer.hpp"
#include "core/db/source_file_info.hpp"
#include "core/key_value_store/compound.hpp"

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
			   get_compiler{[](void const* handle) -> Db::Compiler const& {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return self.compiler();
			   }},
			   set_compiler{[](void* handle, Db::Compiler&& compiler) {
				   auto& self = *reinterpret_cast<T*>(handle);
				   (void)self.compiler(std::move(compiler));
			   }},
			   destroy{[](void* handle) {
				   auto self = reinterpret_cast<T*>(handle);
				   delete self;
			   }},
			   to_json{[](void const* handle) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return toJson(self);
			   }},
			   clone{[](void const* handle) -> void* {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return new T(self);
			   }}
			{
			}

			void (*filter_input)(void const* handle,
			                     Io::Reader input,
			                     SourceOutStream source,
			                     TagsOutStream tags);
			std::vector<Db::Dependency> (*get_dependencies)(void const* handle, Io::Reader source_stream);
			Db::Compiler const& (*get_compiler)(void const* handle);
			void (*set_compiler)(void* handle, Db::Compiler&&);
			void (*destroy)(void* handle);
			KeyValueStore::JsonHandle (*to_json)(void const* handle);
			void* (*clone)(void const* handle);
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

		Loader(Loader const& other):
		   m_handle{other.m_vtable.clone(other.m_handle)}, m_vtable{other.m_vtable}
		{
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
		   m_handle{loader.release()}, m_vtable{source_file_loader_detail::Tag<LoaderType>{}}
		{
		}

		void filterInput(Io::Reader input, SourceOutStream source, TagsOutStream tags) const
		{
			assert(valid());
			m_vtable.filter_input(m_handle, input, source, tags);
		}

		std::vector<Db::Dependency> getDependencies(Io::Reader input) const
		{
			assert(valid());
			return m_vtable.get_dependencies(m_handle, input);
		}

		Db::Compiler const& compiler() const
		{
			assert(valid());
			return m_vtable.get_compiler(m_handle);
		}

		Loader& compiler(Db::Compiler&& compiler)
		{
			m_vtable.set_compiler(m_handle, std::move(compiler));
			return *this;
		}

		bool valid() const
		{
			return m_handle != nullptr;
		}

		KeyValueStore::JsonHandle toJson() const
		{
			return m_vtable.to_json(m_handle);
		}

		void test()
		{
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
