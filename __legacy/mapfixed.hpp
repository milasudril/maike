//@	{"targets":[{"name":"mapfixed.hpp","type":"include"}]}

#ifndef MAIKE_MAPFIXED_H
#define MAIKE_MAPFIXED_H

#include "visibility.hpp"
#include <type_traits>
#include <cstddef>
#include <cstdio>
#include <cassert>
#include <algorithm>


namespace Maike
	{
#ifndef DOXYGEN
	// generate a sequence of integers as non-type template arguments
	// (a basic meta-programming tool)
	template<size_t... Is> struct PRIVATE seq {};
	template<size_t N, size_t... Is> struct PRIVATE gen_seq : gen_seq<N-1, N-1, Is...> {};
	template<size_t... Is> struct PRIVATE gen_seq<0, Is...> : seq<Is...> {};

	// an array type that can be returned from a function
	// and has `constexpr` accessors (as opposed to C++11's `std::array`)
	template<class T, size_t N>
	struct PRIVATE c_array
		{
		template<typename... U>
		constexpr explicit c_array(U... values):m{values...}
			{}

		T m[N];

		constexpr T const& operator[](size_t p) const
			{return m[p];}

		constexpr T const* begin() const { return m+0; }
		constexpr T const* end() const { return m+N; }
		};

	// return the index of the smallest element
	template<class T,size_t Size>
	constexpr size_t c_min_index(c_array<T, Size> const& arr, size_t offset,size_t cur)
		{
		return Size==offset? cur:c_min_index(arr, offset+1, arr[cur] < arr[offset] ? cur : offset);
		}

	// copy the array but with the elements at `index0` and `index1` swapped
	template<class T, size_t Size, size_t... Is>
	constexpr c_array<T, Size> c_swap(c_array<T, Size> const& arr,
		size_t index0, size_t index1, seq<Is...>)
		{
		return c_array<T,Size>{arr[Is == index0 ? index1 : Is == index1 ? index0 : Is]...};
		}

	template<class T, size_t Size>
	constexpr c_array<T, Size> c_sel_sort(c_array<T, Size> const& arr, size_t cur = 0)
		{
		return cur == Size ? arr :
			c_sel_sort( c_swap(arr, cur, c_min_index(arr, cur, cur), gen_seq<Size>{}),cur+1 );
		}
#endif
	/**\brief An associative container with compile-time keys.
	*/
	template<class Key,class Value, Key ... args>
	class PRIVATE MapFixed
		{
		public:
			class Iterator
				{
				public:
					bool operator==(const Iterator& b) const noexcept
						{return b.position==position;}

					bool operator!=(const Iterator& b) const noexcept
						{return !(b.position==position);}

					Iterator& operator++() noexcept
						{
						++position;
						return *this;
						}

					explicit operator size_t() const noexcept
						{return position;}

				private:
					explicit Iterator(size_t x):position(x){}
					size_t position;
					friend class MapFixed;
				};

			template<Key key>
			Value& get() noexcept
				{
				return m_values[Find<key>::value];
				}

			template<Key key>
			constexpr const Value& get() const noexcept
				{
				return m_values[Find<key>::value];
				}

			static constexpr size_t length() noexcept
				{return sizeof...(args);}

			static constexpr size_t size() noexcept
				{return sizeof...(args);}



			Value& operator[](const Key& key) noexcept
				{
				auto i=find(key);
				assert(i!=end());
				return m_values[i];
				}

			const Value& operator[](const Key& key) const noexcept
				{
				auto i=find(key);
				assert(i!=end());
				return m_values[i];
				}


			Value& operator[](Iterator i) noexcept
				{
				assert(i!=end());
				return m_values[i.position];
				}

			const Value& operator[](Iterator i) const noexcept
				{
				assert(i!=end());
				return m_values[i.position];
				}



			static Iterator find(const Key& key) noexcept
				{
				auto first = std::lower_bound(keys.begin(),keys.end(),key);
				return (first!=keys.end() && !(key<*first))? Iterator(first - keys.begin())
					:end();
				}

			static constexpr Iterator begin()
				{return Iterator{0};}

			static constexpr Iterator end()
				{return Iterator{sizeof...(args)};}

			template<Key key>
			static constexpr size_t find() noexcept
				{return Find<key>::value;}

			const Value* values() const noexcept
				{return m_values;}

			Value* values() noexcept
				{return m_values;}

		private:
#ifndef DOXYGEN
			static constexpr c_array<Key,sizeof...(args)> keys{c_sel_sort(c_array<Key,sizeof...(args)>( Key(args)... ))};
			Value m_values[sizeof...(args)];

			template<Key key=keys[sizeof...(args)-1],size_t N=sizeof...(args),bool done=0>
			struct PRIVATE Unique
				{typedef typename Unique<keys[N-2], N-1, !(keys[N - 2] < key)>::unique_tag unique_tag;};

			template<Key key,size_t N>
			struct PRIVATE Unique<key,N,1>
				{};

			template<Key key>
			struct PRIVATE Unique<key,1,0>
				{typedef bool unique_tag;};

			typedef typename Unique<>::unique_tag unique_tag;

			template<Key key,size_t first=0,size_t count=sizeof...(args) - first>
			struct PRIVATE LowerBound
				{
				static constexpr size_t it=first;
				static constexpr size_t step=count/2;
				static constexpr size_t pos=it+step;
				static constexpr size_t first_next=keys[pos]<key?pos + 1:first;
				static constexpr size_t count_next=keys[pos]<key?
					count - (step + 1):step;
				static constexpr size_t value=LowerBound<key,first_next,count_next>::value;
				};

			template<Key key,size_t first>
			struct PRIVATE LowerBound<key,first,0>
				{
				static constexpr size_t value=first;
				};

			template<bool x,size_t v>
			struct PRIVATE IfFalse{};

			template<size_t v>
			struct PRIVATE IfFalse<0,v>
				{
				static constexpr size_t value=v;
				};

			template<size_t v>
			struct PRIVATE IfFalse<1,v>
				{
				struct KeyNotFound{};
				static constexpr KeyNotFound value{};
				};

			template<Key key>
			struct PRIVATE Find
				{
				static constexpr size_t value=IfFalse<
					LowerBound<key>::value==sizeof...(args) || key < keys[LowerBound<key>::value],LowerBound<key>::value >::value;
				};
		};
	template<class Key,class Value, Key ... args>
		constexpr c_array<Key,sizeof...(args)> MapFixed<Key,Value,args...>::keys;
#endif
	}

#endif
