//@	{
//@	 "targets":[{"name":"thread_pool.hpp","type":"include"
//@		, "dependencies":[{"ref":"pthread", "rel":"external"}]}]
//@	,"dependencies_extra":[{"ref":"thread_pool.o", "rel":"implementation"}]
//@	}

#ifndef MAIKE_THREADPOOL_HPP
#define MAIKE_THREADPOOL_HPP

#include "./unique_function.hpp"

#include <pthread.h>
#include <sys/sysinfo.h>

#include <algorithm>
#include <memory>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <exception>

namespace Maike
{
	class ThreadPool
	{
	public:
		template<class T>
		class TaskResult
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

			~TaskResult()
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

		explicit ThreadPool(int m_n_threads = std::max(get_nprocs(), 1));

		template<class Function>
		ThreadPool& addTask(Function&& f, TaskResult<std::result_of_t<std::decay_t<Function>()>>& result)
		{
			auto task = UniqueFunction<void()>([do_it = std::move(f), res = std::ref(result)]() mutable {
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

		~ThreadPool();

	private:
		std::queue<UniqueFunction<void()>> m_tasks;
		std::mutex m_mtx;
		std::condition_variable m_cv;
		bool m_terminate;

		int m_n_threads;
		std::unique_ptr<pthread_t[]> m_threads;

		void performTasks();
	};
}

#endif