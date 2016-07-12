//@	{
//@	 "targets":[{"name":"resourceobject.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"resourceobject.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_RESOURCEOBJECT_H
#define MAIKE_RESOURCEOBJECT_H

#include <utility>
#include <cstddef>

namespace Maike
	{
	class DataSource;
	class DataSink;

	class ResourceObject
		{
		public:
			class Iterator
				{
				public:
					Iterator(const Iterator&)=delete;
					Iterator& operator=(const Iterator&)=delete;

					Iterator(Iterator&& i) noexcept:
						r_object(i.r_object),m_handle(i.m_handle)
						{i.m_handle=nullptr;}
					Iterator& operator=(Iterator&& i)=delete;



					Iterator(const ResourceObject& object);
					Iterator(ResourceObject&& object)=delete;
					void next() noexcept;
					std::pair<const char*,ResourceObject> get() const noexcept;
					bool endAt() noexcept;

				private:
					const ResourceObject& r_object;
					void* m_handle;

				};

			ResourceObject(const ResourceObject&)=delete;
			ResourceObject& operator=(const ResourceObject&)=delete;

			explicit ResourceObject(DataSource&& reader):ResourceObject(reader)
				{}

			explicit ResourceObject(DataSource& reader);

			explicit ResourceObject(long long int x);
			explicit ResourceObject(const char* str);
			explicit ResourceObject(double x);

			enum class Type:unsigned int{OBJECT,ARRAY,STRING,INTEGER,FLOAT};
			explicit ResourceObject(Type type);

			~ResourceObject();

			ResourceObject(ResourceObject&& tree) noexcept:m_handle(tree.m_handle)
				{tree.m_handle=nullptr;}

			ResourceObject& operator=(ResourceObject&& tree) noexcept
				{
				std::swap(tree.m_handle,m_handle);
				return *this;
				}

			Type typeGet() const noexcept;

			ResourceObject objectGet(const char* name) const;
			bool objectExists(const char* name) const noexcept;
			size_t objectCountGet() const noexcept;
			ResourceObject objectGet(size_t index) const;
			Iterator objectIteratorGet() const noexcept
				{
				Iterator i(*this);
				return std::move(i);
				}

			const char* stringGet() const noexcept;
			long long int integerGet() const noexcept;
			double floatGet() const noexcept;

			explicit operator const char*() const;
			explicit operator long long int() const;
			explicit operator double() const;

			ResourceObject& objectAppend(ResourceObject&& object);
			ResourceObject& objectAppend(const ResourceObject& object)=delete;
			ResourceObject& objectSet(const char* key,ResourceObject&& object);
			ResourceObject& objectSet(const ResourceObject& object)=delete;

			void write(DataSink&& sink) const
				{writeImpl(sink);}

			void write(DataSink& sink) const
				{writeImpl(sink);}

		private:
			ResourceObject(void* handle,const char* name);
			void* m_handle;

			void writeImpl(DataSink& sink) const;
		};
	};

#endif
