//@	{
//@	"dependencies_extra":[{"ref":"./thread_pool.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_SCHED_THREADPOOL_HPP
#define MAIKE_SCHED_THREADPOOL_HPP

#include "./thread_count.hpp"

#include "core/utils/unique_function.hpp"

#include <pthread.h>
#include <sys/sysinfo.h>

#include <algorithm>
#include <memory>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <optional>

namespace Maike::Sched
{
	template<class T>
	class AsyncTaskResult
	{
	public:
		T const& get() const
		{
			std::unique_lock<std::mutex> lock{m_mtx};
			m_cv.wait(lock, [this]() { return ready(); });
			if(m_except) { std::rethrow_exception(m_except); }
			return *m_result;
		}

		T take()
		{
			std::unique_lock<std::mutex> lock{m_mtx};
			m_cv.wait(lock, [this]() { return ready(); });
			if(m_except) { std::rethrow_exception(m_except); }
			return std::move(*m_result);
		}

		void set(T&& obj)
		{
			std::lock_guard lock{m_mtx};
			m_result = std::move(obj);
			m_cv.notify_one();
		}

		void set(std::exception_ptr except)
		{
			std::lock_guard lock{m_mtx};
			m_except = except;
			m_cv.notify_one();
		}

		~AsyncTaskResult()
		{
			std::unique_lock<std::mutex> lock{m_mtx};
			m_cv.wait(lock, [this]() { return ready(); });
		}

	private:
		std::optional<T> m_result;
		std::exception_ptr m_except;
		mutable std::mutex m_mtx;
		mutable std::condition_variable m_cv;

		bool ready() const
		{
			return m_result.has_value() || m_except;
		}
	};

	class ThreadPool
	{
	public:
		template<class T>
		using TaskResult = AsyncTaskResult<T>;

		explicit ThreadPool(ThreadCount n_threads = ThreadCount{});

		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		template<class Function>
		ThreadPool& addTask(Function&& f, TaskResult<std::result_of_t<std::decay_t<Function>()>>& result)
		{
			auto task =
			   UniqueFunction<void()>([do_it = std::forward<Function>(f), res = std::ref(result)]() mutable {
				   try
				   {
					   res.get().set(do_it());
				   }
				   catch(...)
				   {
					   res.get().set(std::current_exception());
				   }
			   });
			{
				std::lock_guard lock{m_mtx};
				m_tasks.push(std::move(task));
				m_cv.notify_one();
			}
			return *this;
		}

		template<class Function>
		ThreadPool& addTask(
		   Function&& f,
		   std::enable_if_t<std::is_same_v<std::result_of_t<std::decay_t<Function>()>, void>, int> = 0)
		{
			auto task = UniqueFunction<void()>([do_it = std::forward<Function>(f), this]() mutable {
				try
				{
					do_it();
				}
				catch(...)
				{
					std::lock_guard lock{m_eptr_mtx};
					m_eptr = (m_eptr == nullptr) ? std::current_exception() : m_eptr;
				}
			});
			{
				std::lock_guard lock{m_mtx};
				m_tasks.push(std::move(task));
				m_cv.notify_one();
			}
			return *this;
		}

		std::exception_ptr takeLastException()
		{
			std::lock_guard lock{m_eptr_mtx};
			auto ret = m_eptr;
			m_eptr = nullptr;
			return ret;
		}

		~ThreadPool();

		size_t count() const
		{
			return m_n_threads;
		}

	private:
		std::queue<UniqueFunction<void()>> m_tasks;
		std::mutex m_mtx;
		std::condition_variable m_cv;
		bool m_terminate;

		int m_n_threads;
		std::unique_ptr<pthread_t[]> m_threads;

		void performTasks();

		std::mutex m_eptr_mtx;
		std::exception_ptr m_eptr;
	};
}

#endif
