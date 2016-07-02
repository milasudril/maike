//@	{
//@	"targets":[ {"name":"datasource.hpp","type":"include"} ]
//@	}

#ifndef MAIKE_DATASOURCE_HPP
#define MAIKE_DATASOURCE_HPP

#include <cstddef>
#include <memory>

namespace Maike
	{
	class DataSource
		{
		public:
			virtual size_t read(void* buffer,size_t n)=0;
			virtual const char* nameGet() const noexcept=0;

		protected:
			~DataSource()=default;
		private:
			friend class DataSourceHandle;

			virtual void destroy()=0;

			static void destroy(DataSource* self) noexcept
				{self->destroy();}
		};

	struct DataSourceHandle:public std::unique_ptr<DataSource,void(*)(DataSource*)>
		{
		DataSourceHandle(DataSource* source):
			std::unique_ptr<DataSource,void(*)(DataSource*)>(source,DataSource::destroy)
			{}
		};
	};

#endif
