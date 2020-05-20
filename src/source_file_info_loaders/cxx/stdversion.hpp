//@	{
//@	 "targets":[{"name":"stdversion.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"stdversion.o","rel":"implementation"}]
//@	}

#ifndef CXX_STDVERSION_HPP
#define CXX_STDVERSION_HPP

#include "src/command.hpp"

namespace Cxx
{
	class Stdversion
	{
	public:
		Stdversion(): m_value{-1}
		{
		}

		explicit Stdversion(char const* name);

		char const* c_str() const;

		long value() const
		{
			return m_value;
		}

		bool valid() const
		{
			return m_value != -1;
		}


	private:
		long m_value;
	};

	inline bool operator<(Stdversion a, Stdversion b)
	{
		return a.value() < b.value();
	}

	inline bool operator>(Stdversion a, Stdversion b)
	{
		return a.value() > b.value();
	}

	inline bool operator==(Stdversion a, Stdversion b)
	{
		return a.value() == b.value();
	}

	inline bool operator!=(Stdversion a, Stdversion b)
	{
		return !(a == b);
	}

	inline bool operator<=(Stdversion a, Stdversion b)
	{
		return !(a > b);
	}

	inline bool operator>=(Stdversion a, Stdversion b)
	{
		return !(a < b);
	}


	inline auto fromJson(Maike::KeyValueStore::Empty<Stdversion>,
	                     Maike::KeyValueStore::JsonRefConst ref)
	{
		return Stdversion{ref.as<char const*>()};
	}

	inline auto toJson(Stdversion ver)
	{
		return Maike::KeyValueStore::JsonHandle{ver.c_str()};
	}

}

#endif