//@	{
//@	"targets":[ {"name":"datasink.hpp","type":"include"} ]
//@	}

#ifndef MAIKE_DATASINK_HPP
#define MAIKE_DATASINK_HPP

#include <cstddef>
#include <memory>

namespace Maike
	{
	class DataSink
		{
		public:
			virtual size_t write(const void* buffer,size_t n)=0;

		protected:
			~DataSink()=default;

		private:
			friend class DataSinkHandle;

			virtual void destroy()=0;
			static void destroy(DataSink* self) noexcept
				{self->destroy();}
		};

	struct DataSinkHandle:public std::unique_ptr<DataSink,void(*)(DataSink*)>
		{
		DataSinkHandle(DataSink* sink):
			std::unique_ptr<DataSink,void(*)(DataSink*)>(sink,DataSink::destroy)
			{}
		};
	};

#endif
