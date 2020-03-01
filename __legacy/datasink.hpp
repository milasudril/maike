//@	{
//@	"targets":[ {"name":"datasink.hpp","type":"include"} ]
//@	}

#ifndef MAIKE_DATASINK_HPP
#define MAIKE_DATASINK_HPP

#include "visibility.hpp"
#include <cstddef>
#include <memory>

namespace Maike
{
	class PRIVATE DataSink
	{
	public:
		typedef DataSink Base;

		static void destroy(DataSink* self) noexcept
		{
			self->destroy();
		}

		virtual size_t write(const void* buffer, size_t n) = 0;

	protected:
		~DataSink() = default;

	private:
		virtual void destroy() = 0;
	};
};

#endif
