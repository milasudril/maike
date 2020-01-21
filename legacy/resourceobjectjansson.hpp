//@	{
//@	 "targets":[{"name":"resourceobjectjansson.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"resourceobjectjansson.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_RESOURCEOBJECTJANSSON_HPP
#define MAIKE_RESOURCEOBJECTJANSSON_HPP

#include "resourceobject.hpp"

namespace Maike
	{
	class DataSource;

	class PRIVATE ResourceObjectJansson:public ResourceObject
		{
		public:
			explicit ResourceObjectJansson(DataSource&& reader):
				ResourceObjectJansson(reader)
				{}

			explicit ResourceObjectJansson(DataSource& reader);
			explicit ResourceObjectJansson(long long int x);
			explicit ResourceObjectJansson(const char* str);
			explicit ResourceObjectJansson(double x);
			explicit ResourceObjectJansson(Type type);

			static ResourceObject createImpl(DataSource& source);

		private:
			class PRIVATE Iterator:public ResourceObject::Iterator
				{
				using ResourceObject::Iterator::Iterator;

				friend class ResourceObjectJansson;
				static const Vtable s_vtable;

				static void destroyImpl(void* handle);
				static void* nextImpl(void* handle,void* owner);
				static std::pair<const char*,ResourceObject> getImpl(void* handle);
				static bool endAtImpl(const void* handle);
				};
			friend class Iterator;

			explicit ResourceObjectJansson(void* handle);
			static void destroy(void* handle);

			template<class T>
			static ResourceObject createImpl(T data);
			static ResourceObject createObjectImpl();
			static ResourceObject createArrayImpl();
			static Type typeGetImpl(const void* handle);
			static bool objectExistsImpl(const void* handle,const char* key);
			static ResourceObject objectGetImpl(const void* handle,const char* key);
			static ResourceObject::Iterator objectIteratorGetImpl(void* handle);
			static size_t objectCountGetImpl(const void* handle);
			static ResourceObject objectGetImpl(const void* handle,size_t index);
			static const char* stringGetImpl(const void* handle);
			static long long int integerGetImpl(const void* handle);
			static double floatGetImpl(const void* handle);
			static void objectAppendImpl(void* handle,ResourceObject&& obj);
			static void objectSetImpl(void* handle,const char* key,ResourceObject&& obj);
			static void writeImpl(const void* handle,DataSink& sink);

			static const Vtable s_vtable;
			
		};
	}

#endif