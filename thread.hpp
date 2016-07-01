//@	{
//@	 "targets":[{"name":"thread.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"thread.o","rel":"implementation"}]
//@	}

#ifndef MAIKE_THREAD_H
#define MAIKE_THREAD_H

#include <cstdint>
#include <utility>

namespace Maike
	{
	class ThreadBase
		{
		public:
			virtual void run()=0;

		protected:
			ThreadBase();
			~ThreadBase();

			void start();

		private:
			intptr_t m_handle;
		};

	template<class Runner>
	class Thread:private ThreadBase
		{
		public:
			explicit Thread(const Runner& runner):m_runner(runner)
				{start();}

			explicit Thread(Runner&& runner):m_runner(std::move(runner))
				{start();}

		private:
			void run()
				{m_runner();}
			Runner m_runner;
		};
	}

#endif
