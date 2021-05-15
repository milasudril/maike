//@	{
//@	}

#ifndef MAIKE_SCHED_TASKCOUNTER_HPP
#define MAIKE_SCHED_TASKCOUNTER_HPP

namespace Maike::Sched
{
	template<class Tag>
	class TaskCounter
	{
	public:
		constexpr TaskCounter(): m_n{0}
		{
		}

		constexpr size_t value() const
		{
			return m_n;
		}

		constexpr TaskCounter& operator++()
		{
			++m_n;
			return *this;
		}

		constexpr TaskCounter operator++(int)
		{
			auto ret = *this;
			++(*this);
			return ret;
		}

		constexpr TaskCounter& operator+=(TaskCounter other)
		{
			m_n += other.m_n;
			return *this;
		}

	private:
		size_t m_n;
	};

	template<class Tag>
	constexpr bool operator==(TaskCounter<Tag> a, TaskCounter<Tag> b)
	{
		return a.value() == b.value();
	}

	template<class Tag>
	constexpr bool operator!=(TaskCounter<Tag> a, TaskCounter<Tag> b)
	{
		return !(a == b);
	}
}

#endif
