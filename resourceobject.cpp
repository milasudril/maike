//	{
//	"targets":
//		[{
//		 "name":"resourceobject.o","type":"object"
//		,"dependencies":[{"ref":"jansson","rel":"external"}]
//		}]
//	}

#include "resourceobject.hpp"
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

ResourceObject::Iterator::Iterator(const ResourceObject& object):r_object(object)
	{
	m_handle=json_object_iter(static_cast<json_t*>(object.m_handle));
	}

std::pair<const char*,ResourceObject> ResourceObject::Iterator::get() const noexcept
	{
	auto key=json_object_iter_key(m_handle);
	ResourceObject value{json_object_iter_value(m_handle),key};

	return {key,std::move(value)};
	}

void ResourceObject::Iterator::next() noexcept
	{
	m_handle=json_object_iter_next(static_cast<json_t*>(r_object.m_handle),m_handle);
	}

bool ResourceObject::Iterator::endAt() noexcept
	{
	return m_handle==nullptr;
	}



ResourceObject::ResourceObject(DataSource& readhandler)
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

ResourceObject::ResourceObject(long long int x)
	{
	m_handle=json_integer(x);
	if(m_handle==nullptr)
		{exceptionRaise(ErrorMessage("Failed to store #0;",{static_cast<int64_t>(x)}));}
	}

ResourceObject::ResourceObject(double x)
	{
	m_handle=json_real(x);
	if(m_handle==nullptr)
		{exceptionRaise(ErrorMessage("Failed to store #0;",{x}));}
	}

ResourceObject::ResourceObject(const char* str)
	{
	m_handle=json_string(str);
	if(m_handle==nullptr)
		{exceptionRaise(ErrorMessage("Failed to store #0;",{str}));}
	}

ResourceObject::ResourceObject(Type type)
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



ResourceObject::ResourceObject(void* handle,const char* name)
	{
	m_handle=handle;
	json_incref(static_cast<json_t*>(m_handle));
	}

ResourceObject::~ResourceObject() noexcept
	{
	if(m_handle!=nullptr)
		{json_decref(static_cast<json_t*>(m_handle));}
	}



ResourceObject::Type ResourceObject::typeGet() const noexcept
	{
	switch(json_typeof(static_cast<const json_t*>(m_handle)))
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

ResourceObject ResourceObject::objectGet(const char* name) const
	{
	auto result=json_object_get(static_cast<const json_t*>(m_handle),name);
	if(result==NULL)
		{
		exceptionRaise(ErrorMessage("Could not get child object \"#0;\".",{name}));
		}

	return ResourceObject(result,name);
	}

bool ResourceObject::objectExists(const char* name) const noexcept
	{
	return json_object_get(static_cast<const json_t*>(m_handle),name)!=NULL;
	}

size_t ResourceObject::objectCountGet() const noexcept
	{return json_array_size(static_cast<const json_t*>(m_handle));}

ResourceObject ResourceObject::objectGet(size_t index) const
	{
	auto result=json_array_get(static_cast<const json_t*>(m_handle),index);
	if(result==NULL)
		{
		exceptionRaise(ErrorMessage("Could not get child object number #0;.",{index + 1}));
		}
	return ResourceObject(result,nullptr);
	}

const char* ResourceObject::stringGet() const noexcept
	{
	auto ret=json_string_value(static_cast<const json_t*>(m_handle));
	if(ret==NULL)
		{return nullptr;}
	return ret;
	}

long long int ResourceObject::integerGet() const noexcept
	{return json_integer_value(static_cast<const json_t*>(m_handle));}

double ResourceObject::floatGet() const noexcept
	{
	if(typeGet()==Type::INTEGER)
		{
	//	logWrite(Log::MessageType::WARNING
		//	,"Reading floating point value encoded as integer.",{});
		return static_cast<double>(integerGet());
		}
	return json_real_value(static_cast<const json_t*>(m_handle));
	}

ResourceObject::operator const char*() const
	{
	auto ret=stringGet();
	if(ret==nullptr)
		{
		exceptionRaise(ErrorMessage("Current resource object is not a string.",{}));
		}
	return ret;
	}

ResourceObject::operator long long int() const
	{
	if(typeGet()!=Type::INTEGER)
		{
		exceptionRaise(ErrorMessage("Current resource object is not an integer.",{}));
		}
	return integerGet();
	}

ResourceObject::operator double() const
	{
	switch(typeGet())
		{
		case Type::INTEGER:
		case Type::FLOAT:
			return floatGet();

		default:
			exceptionRaise(ErrorMessage("Current resource object is not an integer.",{}));
		}
	}

ResourceObject& ResourceObject::objectAppend(ResourceObject&& object)
	{
	if(json_array_append_new(static_cast<json_t*>(m_handle)
		,static_cast<json_t*>(object.m_handle))==-1)
		{exceptionRaise(ErrorMessage("It was not possible to append another object to a JSON array",{}));}
	object.m_handle=nullptr;
	return *this;
	}

ResourceObject& ResourceObject::objectSet(const char* key,ResourceObject&& object)
	{
	if(json_object_set_new(static_cast<json_t*>(m_handle),key
		,static_cast<json_t*>(object.m_handle))==-1)
		{exceptionRaise(ErrorMessage("It was not possible to append an new object with key #0;",{key}));}
	object.m_handle=nullptr;
	return *this;
	}

void ResourceObject::writeImpl(DataSink& sink) const
	{
	json_dump_callback(static_cast<const json_t*>(m_handle),storeCallback,&sink
		,JSON_COMPACT|JSON_INDENT(4)|JSON_SORT_KEYS);
	}
