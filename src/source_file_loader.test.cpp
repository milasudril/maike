//@	{
//@	  "targets":[{"name":"source_file_loader.test","type":"application", "autorun":1}]
//@	 }

#undef NDEBUG

#include "./source_file_loader.hpp"

#include <cassert>

namespace
{
	class CallResult
	{};

	struct SourceFileLoaderStub
	{
		std::reference_wrapper<CallResult> res;
	};

	void filterInput(SourceFileLoaderStub const&, Maike::Reader, Maike::SourceOutStream, Maike::TagsOutStream)
	{}

	std::vector<Maike::Dependency> loadDependencies(SourceFileLoaderStub const&, Maike::Reader)
	{
		return std::vector<Maike::Dependency>{};
	}

	Maike::Compiler getCompiler(SourceFileLoaderStub const&, Maike::KeyValueStore::CompoundRefConst)
	{
		return Maike::Compiler{};
	}
}

namespace Testcases
{
	void maikeSourceFileLoaderCreateFromObject()
	{
		CallResult res;
		Maike::SourceFileLoader obj{SourceFileLoaderStub{res}};
		assert(obj.valid());
	}

	void maikeSourceFileLoaderCreateFromUniquePtr()
	{
	}
}

int main()
{
	Testcases::maikeSourceFileLoaderCreateFromObject();
	Testcases::maikeSourceFileLoaderCreateFromUniquePtr();
}


#ifndef MAIKE_SOURCEFILELOADER_HPP
#define MAIKE_SOURCEFILELOADER_HPP

#include "./compiler.hpp"
#include "./reader.hpp"
#include "./tag_filter.hpp"
#include "key_value_store/compound.hpp"

#include <type_traits>

namespace Maike
{
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

		template<class Loader, std::enable_if_t<!std::is_same_v<std::decay_t<Loader>, SourceFileLoader>>>
		explicit SourceFileLoader(Loader&& loader):
		   m_handle{new Loader(std::forward<Loader>(loader))},
		   m_vtable{Tag<Loader>{}}
		{
		}

		template<class Loader, std::enable_if_t<!std::is_same_v<std::decay_t<Loader>, SourceFileLoader>>>
		explicit SourceFileLoader(std::unique_ptr<Loader> loader):
		   m_handle{loader.release()},
		   m_vtable{Tag<Loader>{}}
		{
		}

		void filter(Reader input, SourceOutStream source, TagsOutStream tags) const
		{
			m_vtable.run_filter(m_handle, input, source, tags);
		}

		std::vector<Dependency> loadDependencies(Reader input) const
		{
			return m_vtable.load_dependencies(m_handle, input);
		}

		Compiler getCompiler(KeyValueStore::CompoundRefConst cfg) const
		{
			return m_vtable.get_compiler(m_handle, cfg);
		}

	private:
		void* m_handle;

		template<class T>
		struct Tag
		{
		};

		struct Vtable
		{
			template<class T>
			Vtable(Tag<T>):
			   run_filter{[](void const* handle, Reader input, SourceOutStream src, TagsOutStream tags) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   filter(self, input, src, tags);
			   }},
			   load_dependencies{[](void const* handle, Reader input) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return loadDependencies(self, input);
			   }},
			   get_compiler{[](void const* handle, KeyValueStore::CompoundRefConst cfg) {
				   auto const& self = *reinterpret_cast<T const*>(handle);
				   return getCompiler(self, cfg);
			   }},
			   destroy{[](void* handle) {
				   auto& self = *reinterpret_cast<T const*>(handle);
				   delete self;
			   }}
			{
			}

			void (*run_filter)(void const* handle, Reader input, SourceOutStream source, TagsOutStream tags);
			std::vector<Dependency> (*load_dependencies)(void const* handle, Reader source_stream);
			Compiler (*get_compiler)(void const* handle, KeyValueStore::CompoundRefConst cfg);
			void (*destroy)(void* handle);
		};

		Vtable m_vtable;
	};
};

#endif