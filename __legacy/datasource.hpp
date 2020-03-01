//@	{
//@	"targets":[ {"name":"datasource.hpp","type":"include"} ]
//@	}

#ifndef MAIKE_DATASOURCE_HPP
#define MAIKE_DATASOURCE_HPP

#include "visibility.hpp"
#include <cstddef>
#include <memory>

namespace Maike
{
	class PRIVATE DataSource
	{
	public:
		typedef DataSource Base;
		static void destroy(DataSource* self) noexcept
		{
			self->destroy();
		}
		virtual size_t read(void* buffer, size_t n) = 0;
		virtual const char* nameGet() const noexcept = 0;

	protected:
		~DataSource() = default;

	private:
		friend class DataSourceHandle;

		virtual void destroy() = 0;
	};
};

#endif
