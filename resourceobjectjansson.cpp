//@	{
//@	"targets":
//@		[{
//@		 "name":"resourceobjectjansson.o","type":"object"
//@		,"dependencies":[{"ref":"jansson","rel":"external"}]
//@		}]
//@	}

#include "resourceobjectjansson.hpp"
#include "errormessage.hpp"
#include "datasource.hpp"
#include "variant.hpp"
#include "exceptionhandler.hpp"
#include "datasink.hpp"

#include <jansson.h>

using namespace Maike;

static size_t loadCallback(void* buffer, size_t length,void* eventhandler)
	{
	return reinterpret_cast<DataSource*>(eventhandler)->read(buffer,length);
	}

static int storeCallback(const char* buffer,size_t size,void* eventhandler)
	{
	if(reinterpret_cast<DataSink*>(eventhandler)->write(buffer,size)==size)
		{return 0;}
	return -1;
	}



ResourceObjectJansson::ResourceObjectJansson(DataSource& readhandler)
	:ResourceObject(s_vtable)
	{
	json_error_t status;
	m_handle=json_load_callback(loadCallback,&readhandler,0,&status);
	if(m_handle==nullptr)
		{
		if(status.position!=0)
			{
			exceptionRaise(ErrorMessage("#0;:#1;: error: #2;."
				,{readhandler.nameGet(),status.line,status.text}));
			}
		m_handle=json_object();
		}
	}

ResourceObjectJansson::ResourceObjectJansson(const char* str)
	:ResourceObject(s_vtable)
	{
	m_handle=json_string(str);
	if(m_handle==nullptr)
		{exceptionRaise(ErrorMessage("Failed to store #0;",{str}));}
	}

ResourceObjectJansson::ResourceObjectJansson(long long int x)
	:ResourceObject(s_vtable)
	{
	m_handle=json_integer(x);
	if(m_handle==nullptr)
		{exceptionRaise(ErrorMessage("Failed to store #0;",{static_cast<int64_t>(x)}));}
	}


ResourceObjectJansson::ResourceObjectJansson(double x)
	:ResourceObject(s_vtable)
	{
	m_handle=json_real(x);
	if(m_handle==nullptr)
		{exceptionRaise(ErrorMessage("Failed to store #0;",{x}));}
	}

template<class T>
ResourceObject ResourceObjectJansson::createImpl(T data)
	{return ResourceObjectJansson(data);}

ResourceObject ResourceObjectJansson::createImpl(DataSource& source)
	{return ResourceObjectJansson(source);}

ResourceObjectJansson::ResourceObjectJansson(Type type)
	:ResourceObject(s_vtable)
	{
	switch(type)
		{
		case Type::ARRAY:
			m_handle=json_array();
			break;
		case Type::OBJECT:
			m_handle=json_object();
			break;
		default:
			m_handle=nullptr;
		}
	if(m_handle==nullptr)
		{exceptionRaise(ErrorMessage("Failed to create a JSON object",{}));}
	}

ResourceObject ResourceObjectJansson::createObjectImpl()
	{return ResourceObjectJansson(Type::OBJECT);}

ResourceObject ResourceObjectJansson::createArrayImpl()
	{return ResourceObjectJansson(Type::ARRAY);}


ResourceObjectJansson::ResourceObjectJansson(void* handle):
	ResourceObject(s_vtable,handle)
	{
	json_incref(static_cast<json_t*>(m_handle));
	}

void ResourceObjectJansson::destroy(void* handle)
	{
	if(handle!=nullptr)
		{json_decref(static_cast<json_t*>(handle));}
	}



ResourceObject::Type ResourceObjectJansson::typeGetImpl(const void* handle)
	{
	switch(json_typeof(static_cast<const json_t*>(handle)))
		{
		case JSON_OBJECT:
			return Type::OBJECT;

		case JSON_ARRAY:
			return Type::ARRAY;

		case JSON_STRING:
			return Type::STRING;

		case JSON_INTEGER:
			return Type::INTEGER;

		case JSON_REAL:
			return Type::FLOAT;

		default:
			return Type::OBJECT;
		}
	}

ResourceObject ResourceObjectJansson::objectGetImpl(const void* handle,const char* name)
	{
	auto result=json_object_get(static_cast<const json_t*>(handle),name);
	if(result==NULL)
		{
		exceptionRaise(ErrorMessage("Could not get child object \"#0;\".",{name}));
		}

	return ResourceObjectJansson(result);
	}

ResourceObject::Iterator ResourceObjectJansson::objectIteratorGetImpl(void* handle)
	{
	auto iterator=json_object_iter(static_cast<json_t*>(handle));
	return Iterator(Iterator::s_vtable,handle,iterator);
	}

bool ResourceObjectJansson::objectExistsImpl(const void* handle,const char* name)
	{
	return json_object_get(static_cast<const json_t*>(handle),name)!=NULL;
	}

size_t ResourceObjectJansson::objectCountGetImpl(const void* handle)
	{return json_array_size(static_cast<const json_t*>(handle));}

ResourceObject ResourceObjectJansson::objectGetImpl(const void* handle,size_t index)
	{
	auto result=json_array_get(static_cast<const json_t*>(handle),index);
	if(result==NULL)
		{
		exceptionRaise(ErrorMessage("Could not get child object number #0;.",{index + 1}));
		}
	return ResourceObjectJansson(result);
	}

const char* ResourceObjectJansson::stringGetImpl(const void* handle)
	{
	auto ret=json_string_value(static_cast<const json_t*>(handle));
	if(ret==NULL)
		{exceptionRaise(ErrorMessage("Current resource object is not a string.",{}));}
	return ret;
	}

long long int ResourceObjectJansson::integerGetImpl(const void* handle)
	{
	if(typeGetImpl(handle)!=Type::INTEGER)
		{
		exceptionRaise(ErrorMessage("Current resource object is not an integer.",{}));
		}
	return json_integer_value(static_cast<const json_t*>(handle));
	}

double ResourceObjectJansson::floatGetImpl(const void* handle)
	{
	switch(typeGetImpl(handle))
		{
		case Type::INTEGER:
			return static_cast<double>(integerGetImpl(handle));
		case Type::FLOAT:
			return json_real_value(static_cast<const json_t*>(handle));
		default:
			exceptionRaise(ErrorMessage("Current resource object is not an integer.",{}));
		}
	return 0;
	}

void ResourceObjectJansson::objectAppendImpl(void* handle,ResourceObject&& object)
	{
	auto obj=reinterpret_cast<ResourceObjectJansson*>(&object);
	if(json_array_append_new(static_cast<json_t*>(handle)
		,static_cast<json_t*>(obj->m_handle))==-1)
		{exceptionRaise(ErrorMessage("It was not possible to append another object to a JSON array",{}));}
	obj->m_handle=nullptr;
	}

void ResourceObjectJansson::objectSetImpl(void* handle,const char* key,ResourceObject&& object)
	{
	auto obj=reinterpret_cast<ResourceObjectJansson*>(&object);
	if(json_object_set_new(static_cast<json_t*>(handle),key
		,static_cast<json_t*>(obj->m_handle))==-1)
		{exceptionRaise(ErrorMessage("It was not possible to append an new object with key #0;",{key}));}
	obj->m_handle=nullptr;
	}

void ResourceObjectJansson::writeImpl(const void* handle,DataSink& sink)
	{
	json_dump_callback(static_cast<const json_t*>(handle),storeCallback,&sink
		,JSON_COMPACT|JSON_INDENT(4)|JSON_SORT_KEYS);
	}

const ResourceObjectJansson::Vtable ResourceObjectJansson::s_vtable
	{
	 destroy
	,createImpl
	,createImpl
	,createImpl
	,createImpl
	,createObjectImpl
	,createArrayImpl
	,typeGetImpl
	,objectExistsImpl
	,objectGetImpl
	,objectIteratorGetImpl
	,objectCountGetImpl
	,objectGetImpl
	,stringGetImpl
	,integerGetImpl
	,floatGetImpl
	,objectAppendImpl
	,objectSetImpl
	,writeImpl
	};


void ResourceObjectJansson::Iterator::destroyImpl(void*)
	{
//	There is no dtor for iterators
	}

void* ResourceObjectJansson::Iterator::nextImpl(void* handle,void* owner)
	{
	return json_object_iter_next(static_cast<json_t*>(owner),handle);
	}

std::pair<const char*,ResourceObject> ResourceObjectJansson::Iterator::getImpl(void* handle)
	{
	auto key=json_object_iter_key(handle);
	ResourceObjectJansson value(json_object_iter_value(handle));

	return {key,std::move(value)};
	}

bool ResourceObjectJansson::Iterator::endAtImpl(const void* handle)
	{return handle==nullptr;;}

const ResourceObjectJansson::Iterator::Vtable ResourceObjectJansson::Iterator::s_vtable
	{
	 destroyImpl
	,nextImpl
	,getImpl
	,endAtImpl
	};
