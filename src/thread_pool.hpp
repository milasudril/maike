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

namespace Maike
{
	class ThreadPool
	{
	public:
		template<class T>
		class TaskResult
		{
			public:
				T get() const
				{
					std::unique_lock<std::mutex> lock{m_mtx};
					m_cv.wait(lock, [this]() { return m_result.has_value(); });
					return *m_result;
				}

				void set(T&& obj)
				{
					std::lock_guard lock{m_mtx};
					m_result = std::move(obj);
					m_cv.notify_one();
				}

				~TaskResult()
				{
					std::unique_lock<std::mutex> lock{m_mtx};
					m_cv.wait(lock, [this]() { return m_result.has_value(); });
				}

			private:
				std::optional<T> m_result;
				mutable std::mutex m_mtx;
				mutable std::condition_variable m_cv;
		};

		explicit ThreadPool(int m_n_threads = std::max(get_nprocs(), 1));

		template<class Function>
		ThreadPool& addTask(Function&& f, TaskResult<std::result_of_t<std::decay_t<Function>()>>& result)
		{
			auto task = UniqueFunction<void()>(
			   [do_it = std::move(f), res = std::ref(result)]() mutable {
				   res.get().set(do_it());
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