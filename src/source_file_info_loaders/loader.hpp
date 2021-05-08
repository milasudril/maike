//@	{
//@	 }

#ifndef MAIKE_SOURCEFILEINFOLOADERS_LOADER_HPP
#define MAIKE_SOURCEFILEINFOLOADERS_LOADER_HPP

#include "src/io/reader.hpp"
#include "src/io/writer.hpp"
#include "src/db/source_file_info.hpp"
#include "src/key_value_store/compound.hpp"

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
			   destroy{[](void* handle) {
				   auto self = reinterpret_cast<T*>(handle);
				   delete self;
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
			Db::Compiler (*default_compiler)();
			char const* (*name)();
			void (*destroy)(void* handle);
			void* (*clone)(void const* handle);
		};
	}

	class Loader
	{
	public:
		Loader(Loader&& other) noexcept:
		   m_handle{other.m_handle},
		   m_vtable{other.m_vtable},
		   m_compiler{std::move(other.m_compiler)},
		   m_name{std::move(other.m_name)}
		{
			other.m_handle = nullptr;
		}

		Loader& operator=(Loader&& other) noexcept
		{
			std::swap(m_handle, other.m_handle);
			std::swap(m_vtable, other.m_vtable);
			std::swap(m_compiler, other.m_compiler);
			std::swap(m_name, other.m_name);
			other.m_vtable.destroy(other.m_handle);
			other.m_handle = nullptr;
			return *this;
		}

		Loader(Loader const& other):
		   m_handle{other.m_vtable.clone(other.m_handle)},
		   m_vtable{other.m_vtable},
		   m_compiler{other.m_compiler},
		   m_name{other.m_name}
		{
		}

		~Loader()
		{
			m_vtable.destroy(m_handle);
		}

		template<class LoaderType,
		         std::enable_if_t<!std::is_same_v<std::decay_t<LoaderType>, Loader>, int> = 0>
		explicit Loader(LoaderType&& loader,
		                std::optional<Db::Compiler>&& compiler = std::optional<Db::Compiler>{}):
		   m_handle{new LoaderType(std::forward<LoaderType>(loader))},
		   m_vtable{source_file_loader_detail::Tag<LoaderType>{}},
		   m_compiler{compiler ? std::move(*compiler) : LoaderType::defaultCompiler()},
		   m_name{LoaderType::name()}
		{
		}

		template<class LoaderType,
		         std::enable_if_t<!std::is_same_v<std::decay_t<LoaderType>, Loader>, int> = 0>
		explicit Loader(std::unique_ptr<LoaderType> loader,
		                std::optional<Db::Compiler>&& compiler = std::optional<Db::Compiler>{}):
		   m_handle{loader.release()},
		   m_vtable{source_file_loader_detail::Tag<LoaderType>{}},
		   m_compiler{compiler ? std::move(*compiler) : LoaderType::defaultCompiler()},
		   m_name{LoaderType::name()}
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
			return m_compiler;
		}

		Loader& compiler(Db::Compiler&& compiler)
		{
			m_compiler = std::move(compiler);
			return *this;
		}

		std::string const& name() const
		{
			return m_name;
		}

		bool valid() const
		{
			return m_handle != nullptr;
		}

	private:
		void* m_handle;
		source_file_loader_detail::Vtable m_vtable;
		Db::Compiler m_compiler;
		std::string m_name;
	};

	inline auto toJson(Loader const& loader)
	{
		return Maike::KeyValueStore::Compound{}
		   .set("compiler", loader.compiler())
		   .set("loader", loader.name().c_str())
		   .takeHandle();
	}
};

#endif
