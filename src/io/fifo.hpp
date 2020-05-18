//@	{
//@	 "targets":[{"name":"fifo.hpp","type":"include"
//@		, "dependencies":[{"ref":"pthread", "rel":"external"}]}]
//@	}

#ifndef MAIKE_IO_FIFO_HPP
#define MAIKE_IO_FIFO_HPP

#include <list>
#include <mutex>
#include <condition_variable>

namespace Maike::Io
{
	template<class ElemType>
	class Fifo
	{
	public:
		Fifo(): m_bytes_left{0}
		{
		}

		void write(ElemType const* buffer, size_t size)
		{
			auto tmp = std::make_unique<ElemType[]>(size);
			std::copy(buffer, buffer + size, tmp.get());
			std::lock_guard guard{m_mtx};
			m_elems.push_back({size, std::move(tmp)});
			m_cv.notify_one();
		}

		void stop()
		{
			write(nullptr, 0);
		}

		size_t read(ElemType* buffer, size_t size)
		{
			if(m_bytes_left == 0)
			{
				std::unique_lock guard{m_mtx};
				m_cv.wait(guard, [& list = m_elems]() { return list.size() != 0; });
				auto const& front = m_elems.front();
				m_bytes_left = front.first;
				r_read_ptr = front.second.get();
			}
			auto const n = std::min(size, m_bytes_left);
			std::copy(r_read_ptr, r_read_ptr + n, buffer);
			m_bytes_left -= n;
			r_read_ptr += n;
			if(m_bytes_left == 0)
			{
				std::unique_lock guard{m_mtx};
				m_elems.pop_front();
			}
			return n;
		}

	private:
		std::mutex m_mtx;
		std::condition_variable m_cv;
		std::list<std::pair<size_t, std::unique_ptr<ElemType[]>>> m_elems;
		size_t m_bytes_left;
		ElemType const* r_read_ptr;
	};

	template<class ElemType>
	void write(Fifo<ElemType>& fifo, ElemType const* buffer, size_t n)
	{
		fifo.write(buffer, n);
	}

	template<class ElemType>
	size_t read(Fifo<ElemType>& fifo, ElemType* buffer, size_t n)
	{
		return fifo.read(buffer, n);
	}
}

#endif