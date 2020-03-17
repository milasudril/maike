//@	{
//@	 "targets":[{"name":"pingpong_buffer.hpp","type":"include", "dependencies":[{"ref":"pthread", "rel":"external"}]}]
//@	}

#ifndef MAIKE_PINGPONGBUFFER_HPP
#define MAIKE_PINGPONGBUFFER_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <algorithm>

namespace Maike
{
	template<class ElementType>
	class PingPongBuffer
	{
		public:
			explicit PingPongBuffer(size_t N):
				m_capacity{N},
				m_elems_available{0},
				m_eof{false},
				m_write_offset{0}
			{
				m_buffer_a = std::make_unique<ElementType[]>(N);
				m_buffer_b = std::make_unique<ElementType[]>(N);
				r_read_base_ptr = m_buffer_a.get();
				r_write_base_ptr = m_buffer_b.get();
			}

			size_t read(ElementType* output_buffer)
			{
				auto const n = wait();
				std::copy(r_read_base_ptr, r_read_base_ptr + n, output_buffer);
				return n;
			}

			size_t write(ElementType const* input_buffer, size_t N)
			{
				auto const n = std::min(N, m_capacity - m_write_offset);
				std::copy(input_buffer, input_buffer + n, r_write_base_ptr + m_write_offset);
				m_write_offset += n;
				if(m_write_offset == m_capacity)
				{ signal(); }
				return n;
			}

			size_t capacity() const
			{ return m_capacity; }

			void flush()
			{
				{
					std::unique_lock<std::mutex> lock{m_mtx};
					m_eof = true;
				}
				signal();
			}

		private:
			size_t m_capacity;
			size_t m_elems_available;
			bool m_eof;

			mutable std::mutex m_mtx;
			mutable std::condition_variable m_cv;

			size_t wait()
			{
				std::unique_lock<std::mutex> lock{m_mtx};
				m_cv.wait(lock, [this]() { return m_elems_available != 0 || m_eof; });
				auto ret = m_elems_available;
				m_elems_available = 0;
				return ret;
			}

			void signal()
			{
				{
					std::lock_guard<std::mutex> lock{m_mtx};
					std::swap(r_read_base_ptr, r_write_base_ptr);
					m_elems_available = m_write_offset;
					m_cv.notify_one();
				}
				m_write_offset = 0;
			}

			ElementType* r_read_base_ptr;
			ElementType* r_write_base_ptr;
			size_t m_write_offset;
			std::unique_ptr<ElementType[]> m_buffer_a;
			std::unique_ptr<ElementType[]> m_buffer_b;
	};
}

#endif
