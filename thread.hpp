//@	{
//@	 "targets":[{"name":"thread.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"thread.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_THREAD_H
#define MAIKE_THREAD_H

#include "visibility.hpp"
#include <cstdint>
#include <utility>

namespace Maike
	{
	class PRIVATE ThreadBase
		{
		public:
			virtual void run()=0;

		protected:
			ThreadBase();
			~ThreadBase() noexcept;

			void start();
			void synchronize() noexcept;

		private:
			intptr_t m_handle;
		};

	template<class Runner>
	class PRIVATE Thread:private ThreadBase
		{
		public:
			explicit Thread(const Runner& runner):m_runner(runner)
				{start();}

			explicit Thread(Runner&& runner):m_runner(std::move(runner))
				{start();}

			~Thread() noexcept
				{synchronize();}

		private:
			void run()
				{m_runner();}

			Runner m_runner;
		};
	}

#endif
