//@	{"targets":[{"name":"resourceobject.hpp","type":"include"}]}

#ifndef MAIKE_RESOURCEOBJECT_H
#define MAIKE_RESOURCEOBJECT_H

#include "visibility.hpp"
#include <utility>
#include <cstddef>

namespace Maike
	{
	class DataSource;
	class DataSink;

	class PRIVATE ResourceObject
		{
		public:
			class Iterator
				{
				public:
					Iterator(const Iterator&)=delete;
					Iterator& operator=(const Iterator&)=delete;

					Iterator(Iterator&& i) noexcept:r_vtable(i.r_vtable)
						,r_owner(i.r_owner),m_handle(i.m_handle)
						{i.m_handle=nullptr;}

					Iterator& operator=(Iterator&& i)=delete;

					void next() noexcept
						{m_handle=r_vtable->next(m_handle,r_owner);}

					std::pair<const char*,ResourceObject> get() const noexcept
						{return r_vtable->get(m_handle);}

					bool endAt() const noexcept
						{return r_vtable->endAt(m_handle);}

				protected:
					struct Vtable
						{
						void (*destroy)(void* handle);
						void* (*next)(void* handle,void* owner);
						std::pair<const char*,ResourceObject> (*get)(void* handle);
						bool (*endAt)(const void* handle);
						};

					Iterator(const Vtable& vtable,void* owner,void* handle) noexcept:
						r_vtable(&vtable),r_owner(owner),m_handle(handle)
						{}

					const Vtable* r_vtable;
					void* r_owner;
					void* m_handle;
				};

			ResourceObject(const ResourceObject&)=delete;
			ResourceObject& operator=(const ResourceObject&)=delete;

			enum class Type:unsigned int{OBJECT,ARRAY,STRING,INTEGER,FLOAT};

			typedef ResourceObject (*Reader)(DataSource& source);

			ResourceObject create(DataSource& source) const
				{return r_vtable->createSource(source);}

			ResourceObject create(DataSource&& source) const
				{return r_vtable->createSource(source);}

			ResourceObject create(const char* str) const
				{return r_vtable->createString(str);}

			ResourceObject create(long long int x) const
				{return r_vtable->createLongLong(x);}

			ResourceObject create(double x) const
				{return r_vtable->createDouble(x);}

			ResourceObject createObject() const
				{return r_vtable->createObject();}

			ResourceObject createArray() const
				{return r_vtable->createArray();}

			ResourceObject(ResourceObject&& tree) noexcept:r_vtable(tree.r_vtable)
				,m_handle(tree.m_handle)
				{tree.m_handle=nullptr;}

			ResourceObject& operator=(ResourceObject&& tree) noexcept
				{
				std::swap(tree.m_handle,m_handle);
				return *this;
				}

			Type typeGet() const noexcept
				{return r_vtable->typeGet(m_handle);}

			ResourceObject objectGet(const char* name) const
				{return r_vtable->objectGet(m_handle,name);}

			bool objectExists(const char* name) const noexcept
				{return r_vtable->objectExists(m_handle,name);}

			size_t objectCountGet() const noexcept
				{return r_vtable->objectCountGet(m_handle);}

			ResourceObject objectGet(size_t index) const
				{return r_vtable->objectGetIndex(m_handle,index);}

			Iterator objectIteratorGet() const noexcept
				{return r_vtable->objectIteratorGet(m_handle);}

			explicit operator const char*() const
				{return r_vtable->stringGet(m_handle);}
				
			explicit operator long long int() const
				{return r_vtable->integerGet(m_handle);}

			explicit operator double() const
				{return r_vtable->floatGet(m_handle);}

			ResourceObject& objectAppend(ResourceObject&& object)
				{
				r_vtable->objectAppend(m_handle,std::move(object));
				return *this;
				}

			ResourceObject& objectAppend(const ResourceObject& object)=delete;
			ResourceObject& objectSet(const char* key,ResourceObject&& object)
				{
				r_vtable->objectSet(m_handle,key,std::move(object));
				return *this;
				}

			ResourceObject& objectSet(const ResourceObject& object)=delete;

			void write(DataSink&& sink) const
				{r_vtable->write(m_handle,sink);}

			void write(DataSink& sink) const
				{r_vtable->write(m_handle,sink);}

			~ResourceObject()
				{r_vtable->destroy(m_handle);}

		protected:
			struct Vtable
				{
				void (*destroy)(void* handle);
				ResourceObject (*createSource)(DataSource& source);
				ResourceObject (*createString)(const char* str);
				ResourceObject (*createLongLong)(long long int x);
				ResourceObject (*createDouble)(double x);
				ResourceObject (*createObject)();
				ResourceObject (*createArray)();
				Type (*typeGet)(const void* handle);
				bool (*objectExists)(const void* handle,const char* name);
				ResourceObject (*objectGet)(const void* handle,const char* name);
				Iterator (*objectIteratorGet)(void* handle);
				size_t (*objectCountGet)(const void* handle);
				ResourceObject (*objectGetIndex)(const void* handle,size_t index);
				const char* (*stringGet)(const void* handle);
				long long int (*integerGet)(const void* handle);
				double (*floatGet)(const void* handle);

				void (*objectAppend)(void* handle,ResourceObject&& obj);
				void (*objectSet)(void* handle,const char* key,ResourceObject&& obj);
				void (*write)(const void* handle,DataSink& sink);
				};

			ResourceObject(const Vtable& vtable):r_vtable(&vtable),m_handle(nullptr)
				{}

			ResourceObject(const Vtable& vtable,void* handle) noexcept:
				r_vtable(&vtable),m_handle(handle)
				{}

			
			const Vtable* r_vtable;
			void* m_handle;
		};
	};

#endif

